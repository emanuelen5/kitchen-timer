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

#include "graphics.h"

extern "C"
{
#include "avr_ioport.h"
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

uint8_t port_c_state = 0b00000000;

void port_c_changed_hook(struct avr_irq_t *irq, uint32_t value, void *param)
{
    UNUSED irq;
    UNUSED param;
    port_c_state = value;
}

void avr_special_init(avr_t *avr, void *data)
{
    struct avr_flash *flash_data = (struct avr_flash *)data;

    printf("%s\n", __func__);
    flash_data->avr_flash_fd = open(flash_data->avr_flash_path.c_str(),
                                    O_RDWR | O_CREAT, 0644);
    if (flash_data->avr_flash_fd < 0)
    {
        perror(flash_data->avr_flash_path.c_str());
        exit(1);
    }
    (void)ftruncate(flash_data->avr_flash_fd, avr->flashend + 1);
    ssize_t r = read(flash_data->avr_flash_fd, avr->flash, avr->flashend + 1);
    if (r != avr->flashend + 1)
    {
        fprintf(stderr, "unable to load flash memory\n");
        perror(flash_data->avr_flash_path.c_str());
        exit(1);
    }
}

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
    bool interactive;
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
        {"interactive", no_argument, nullptr, 'i'},
        {"verbose", no_argument, nullptr, 'v'},
        {nullptr, 0, nullptr, 0}};

    args_t args = {.debug = false, .verbose = false, .interactive = false, .hex_file = ""};

    int opt;
    while ((opt = getopt_long(argc, argv, "ihdvp:", long_options, nullptr)) != -1)
    {
        switch (opt)
        {
        case 'd':
            args.debug = true;
            break;
        case 'i':
            args.interactive = true;
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

const char *exit_reason(avr_t *avr)
{
    if (avr->state == cpu_Done)
        return "CPU done";

    if (avr->state == cpu_Crashed)
        return "CPU crashed";

    if (avr->pc == 0)
        return "Bootloader finished";

    return nullptr;
}

void run_instructions_until_exited_bootloader(avr_t *avr)
{
    while (true)
    {
        if (exit_reason(avr))
        {
            printf("Exiting: %s\n", exit_reason(avr));
            break;
        }

        avr_run(avr);
    };
    avr_terminate(avr);
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

    avr->codeend = avr->flashend;
    avr->log = 1 + args.verbose;

    avr_irq_register_notify(
        avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), IOPORT_IRQ_PIN_ALL),
        port_c_changed_hook,
        NULL);

    avr->gdb_port = 1234;
    if (args.debug)
    {
        avr->state = cpu_Stopped;
        avr_gdb_init(avr);
    }

    uart_pty_t uart_pty;
    uart_pty_init(avr, &uart_pty);
    uart_pty_connect(&uart_pty, '0');

    if (args.interactive)
    {
        simulate_with_graphics(avr);
    }
    else
    {
        run_instructions_until_exited_bootloader(avr);
    }

    uart_pty_stop(&uart_pty);
}
