#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <string>
#include <cerrno>
#include <iostream>
#include <stdexcept>

#include <GL/glut.h>
#include "pthread.h"

extern "C"
{
#include "avr_ioport.h"
#include "sim_elf.h"
#include "sim_hex.h"
#include "sim_gdb.h"
#include "uart_pty.h"
}

#define UNUSED (void)

struct avr_flash
{
    std::string avr_flash_path;
    int avr_flash_fd;
};

volatile unsigned char key_g;
uint8_t port_c_state = 0b00000000;
const float pixel_size = 64;

void display_callback(void) /* function called whenever redisplay needed */
{
    // OpenGL rendering goes here...
    glClear(GL_COLOR_BUFFER_BIT);

    // Set up modelview matrix
    glMatrixMode(GL_MODELVIEW); // Select modelview matrix
    glLoadIdentity();           // Start with an identity matrix

    const float pixel_size = 64;
    float grid = pixel_size;
    float size = grid * 0.8;
    glBegin(GL_QUADS);
    glColor3f(1, 0, 0);

    for (int di = 0; di < 8; di++)
    {
        const char on = (port_c_state & (1 << di)) != 0;
        if (on)
        {
            float x = (di)*grid;
            float y = 0;
            glVertex2f(x + size, y + size);
            glVertex2f(x, y + size);
            glVertex2f(x, y);
            glVertex2f(x + size, y);
        }
    }

    glEnd();
    glutSwapBuffers();
}

void port_c_pin_changed_hook(struct avr_irq_t *irq, uint32_t value, void *param)
{
    UNUSED param;
    port_c_state = (port_c_state & ~(1 << irq->irq)) | (value << irq->irq);
}

void key_callback(unsigned char key, int x, int y)
{
    UNUSED x;
    UNUSED y;

    const char escape_key = 0x1f;

    switch (key)
    {
    case 'q':
    case escape_key:
        exit(0);
        break;
    default:
        key_g = key;
        break;
    }
}

void timer_callback(int i)
{
    UNUSED i;
    static uint8_t old_port_state = 0b00000000;

    glutTimerFunc(1000 / 64, timer_callback, 0);

    const bool port_changed = old_port_state != port_c_state;
    old_port_state = port_c_state;
    if (port_changed)
        glutPostRedisplay();
}

// avr special flash initalization
// here: open and map a file to enable a persistent storage for the flash memory
void avr_special_init(avr_t *avr, void *data)
{
    struct avr_flash *flash_data = (struct avr_flash *)data;

    printf("%s\n", __func__);
    // open the file
    flash_data->avr_flash_fd = open(flash_data->avr_flash_path.c_str(),
                                    O_RDWR | O_CREAT, 0644);
    if (flash_data->avr_flash_fd < 0)
    {
        perror(flash_data->avr_flash_path.c_str());
        exit(1);
    }
    // resize and map the file
    (void)ftruncate(flash_data->avr_flash_fd, avr->flashend + 1);
    ssize_t r = read(flash_data->avr_flash_fd, avr->flash, avr->flashend + 1);
    if (r != avr->flashend + 1)
    {
        fprintf(stderr, "unable to load flash memory\n");
        perror(flash_data->avr_flash_path.c_str());
        exit(1);
    }
}

// avr special flash deinitalization
// here: cleanup the persistent storage
void avr_special_deinit(avr_t *avr, void *data)
{
    struct avr_flash *flash_data = (struct avr_flash *)data;

    printf("%s\n", __func__);
    lseek(flash_data->avr_flash_fd, SEEK_SET, 0);
    ssize_t r = write(flash_data->avr_flash_fd, avr->flash, avr->flashend + 1);
    if (r != avr->flashend + 1)
    {
        fprintf(stderr, "unable to flush flash memory\n");
        perror(flash_data->avr_flash_path.c_str());
    }
    close(flash_data->avr_flash_fd);
}

void connect_hooks_to_dump_flash_to_file(avr_t *avr, struct avr_flash *flash_data)
{
    flash_data->avr_flash_path = std::string(avr->mmcu) + ".flash.bin";
    flash_data->avr_flash_fd = 0;

    avr->custom.init = avr_special_init;
    avr->custom.deinit = avr_special_deinit;
    avr->custom.data = flash_data;
}

typedef struct fuses
{
    uint8_t lfuse, hfuse, efuse;
} fuses_t;

void setup_fuses(avr_t *avr, fuses_t fuses)
{
    avr->fuse[0] = fuses.lfuse;
    avr->fuse[1] = fuses.hfuse;
    avr->fuse[2] = fuses.efuse;
}

bool endswith(const char *s, const char *suffix)
{
    return strcmp(s + strlen(s) - strlen(suffix), suffix) == 0;
}

typedef struct args
{
    bool debug;
    bool verbose;
    std::string hex_file;
} args_t;

class ArgumentError : public std::runtime_error
{
public:
    explicit ArgumentError(const std::string &message)
        : std::runtime_error(message) {}
};

void print_usage(std::string program_name)
{
    std::cerr << "Usage: " + program_name + " [-d|--debug] [-v|--verbose] hex_file";
}

args_t
parse_arguments(int argc, char *argv[])
{
    const struct option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"debug", no_argument, nullptr, 'd'},
        {"verbose", no_argument, nullptr, 'v'},
        {nullptr, 0, nullptr, 0}};

    args_t args = {.debug = false, .verbose = false, .hex_file = ""};

    int opt;
    while ((opt = getopt_long(argc, argv, "hdvp:", long_options, nullptr)) != -1)
    {
        switch (opt)
        {
        case 'd':
            args.debug = true;
            break;
        case 'v':
            args.verbose = true;
            break;
        case 'h':
        {
            const std::string program_name = std::string(argv[0]);
            print_usage(program_name);
            exit(0);
            break;
        }
        default:
            throw ArgumentError("Unknown flag '" + std::to_string(opt) + "'");
            break;
        }
    }

    int arguments_left = argc - optind;

    if (arguments_left <= 0)
        throw ArgumentError("hex_file argument is required.");
    if (arguments_left > 1)
        throw ArgumentError("Too many arguments.");

    if (!endswith(argv[optind], ".hex"))
        throw ArgumentError("hex_file isn't a .hex file");

    args.hex_file = argv[optind];

    return args;
}

void fill_avr_flash_or_exit(avr_t *avr, std::string hex_file)
{
    uint32_t boot_base, boot_size;

    uint8_t *boot = read_ihex_file(hex_file.c_str(), &boot_size, &boot_base);
    if (!boot)
    {
        fprintf(stderr, "Unable to load %s\n", hex_file.c_str());
        exit(1);
    }
    printf("%s bootloader 0x%05x: %d bytes\n", avr->mmcu, boot_base, boot_size);

    memcpy(avr->flash + boot_base, boot, boot_size);
    free(boot);

    avr->pc = boot_base;
}

static void *
avr_run_thread(void *avr_ptr)
{
    avr_t *avr = (avr_t *)avr_ptr;
    int state = cpu_Running;

    while (true)
    {
        if ((state == cpu_Done) || (state == cpu_Crashed))
            break;

        if (avr->pc == 0)
        {
            printf("Bootloader finished. Exiting.\n");
            break;
        }

        if (key_g != 0)
        {
            printf("key pressed: %c\n", key_g);
            key_g = 0;
        }
        state = avr_run(avr);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    args_t args;
    try
    {
        args = parse_arguments(argc, argv);
    }
    catch (const ArgumentError &e)
    {
        const std::string program_name = std::string(argv[0]);
        std::cerr << "Argument error: " << e.what() << "\n";
        print_usage(program_name);
        return 1;
    }

    std::string mmcu = "atmega328p";
    const uint32_t freq = 1000000;

    avr_t *avr = avr_make_mcu_by_name(mmcu.c_str());
    if (!avr)
    {
        fprintf(stderr, "%s: Error creating the AVR core\n", argv[0]);
        exit(1);
    }

    setup_fuses(
        avr,
        (fuses_t){
            .lfuse = 0x62,
            .hfuse = 0xD0,
            .efuse = 0xFF,
        });

    struct avr_flash flash_data;
    connect_hooks_to_dump_flash_to_file(avr, &flash_data);

    avr_init(avr);
    avr->frequency = freq;

    fill_avr_flash_or_exit(avr, args.hex_file);

    /* end of flash, remember we are writing /code/ */
    avr->codeend = avr->flashend;
    avr->log = 1 + args.verbose;

    // connect all the pins on port B to our callback
    for (int i = 0; i < 8; i++)
        avr_irq_register_notify(
            avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), i),
            port_c_pin_changed_hook,
            NULL);

    // even if not setup at startup, activate gdb if crashing
    avr->gdb_port = 1234;
    if (args.debug)
    {
        avr->state = cpu_Stopped;
        avr_gdb_init(avr);
    }

    uart_pty_t uart_pty;
    uart_pty_init(avr, &uart_pty);
    uart_pty_connect(&uart_pty, '0');

    printf("Demo launching: 'LED' bar is PORTC\n"
           "   Press 'q' to quit\n");

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(8 * pixel_size, 1 * pixel_size); /* width=400pixels height=500pixels */
    glutCreateWindow("Glut");

    // Set up projection matrix
    glMatrixMode(GL_PROJECTION); // Select projection matrix
    glLoadIdentity();            // Start with an identity matrix
    glOrtho(0, 8 * pixel_size, 0, 1 * pixel_size, 0, 10);
    glScalef(1, -1, 1);
    glTranslatef(0, -1 * pixel_size, 0);

    glutDisplayFunc(display_callback);
    glutKeyboardFunc(key_callback);
    glutTimerFunc(1000 / 24, timer_callback, 0);

    // the AVR run on it's own thread. it even allows for debugging!
    pthread_t run;
    pthread_create(&run, NULL, avr_run_thread, (void *)avr);

    glutMainLoop();

    uart_pty_stop(&uart_pty);
}
