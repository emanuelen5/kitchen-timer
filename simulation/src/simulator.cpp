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

extern "C"
{
#include "sim_elf.h"
#include "sim_hex.h"
#include "sim_gdb.h"
#include "uart_pty.h"
}

struct avr_flash
{
    std::string avr_flash_path;
    int avr_flash_fd;
};

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

    struct avr_flash flash_data;
    uint32_t boot_base, boot_size;
    std::string mmcu = "atmega328p";
    const uint32_t freq = 1000000;

    uint8_t *boot = read_ihex_file(args.hex_file.c_str(), &boot_size, &boot_base);
    if (!boot)
    {
        fprintf(stderr, "%s: Unable to load %s\n", argv[0], args.hex_file.c_str());
        exit(1);
    }
    printf("%s bootloader 0x%05x: %d bytes\n", mmcu.c_str(), boot_base, boot_size);

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

    flash_data.avr_flash_path = mmcu + ".flash.bin";
    flash_data.avr_flash_fd = 0;
    // register our own functions
    avr->custom.init = avr_special_init;
    avr->custom.deinit = avr_special_deinit;
    avr->custom.data = &flash_data;
    avr_init(avr);
    avr->frequency = freq;

    memcpy(avr->flash + boot_base, boot, boot_size);
    free(boot);
    avr->pc = boot_base;
    /* end of flash, remember we are writing /code/ */
    avr->codeend = avr->flashend;
    avr->log = 1 + args.verbose;

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

    while (1)
    {
        int state = avr_run(avr);
        if (state == cpu_Done || state == cpu_Crashed)
            break;
    }
    uart_pty_stop(&uart_pty);
}
