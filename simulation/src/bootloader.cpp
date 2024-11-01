#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern "C"
{
#include "sim_elf.h"
#include "sim_hex.h"
#include "sim_gdb.h"
#include "uart_pty.h"
}

uart_pty_t uart_pty;

struct avr_flash
{
    char avr_flash_path[1024];
    int avr_flash_fd;
};

// avr special flash initalization
// here: open and map a file to enable a persistent storage for the flash memory
void avr_special_init(avr_t *avr, void *data)
{
    struct avr_flash *flash_data = (struct avr_flash *)data;

    printf("%s\n", __func__);
    // open the file
    flash_data->avr_flash_fd = open(flash_data->avr_flash_path,
                                    O_RDWR | O_CREAT, 0644);
    if (flash_data->avr_flash_fd < 0)
    {
        perror(flash_data->avr_flash_path);
        exit(1);
    }
    // resize and map the file the file
    (void)ftruncate(flash_data->avr_flash_fd, avr->flashend + 1);
    ssize_t r = read(flash_data->avr_flash_fd, avr->flash, avr->flashend + 1);
    if (r != avr->flashend + 1)
    {
        fprintf(stderr, "unable to load flash memory\n");
        perror(flash_data->avr_flash_path);
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
        fprintf(stderr, "unable to load flash memory\n");
        perror(flash_data->avr_flash_path);
    }
    close(flash_data->avr_flash_fd);
    uart_pty_stop(&uart_pty);
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

int main(int argc, char *argv[])
{
    struct avr_flash flash_data;
    char boot_path[1024] = "ATmegaBOOT_168_atmega328.ihex";
    uint32_t boot_base, boot_size;
    const char *mmcu = "atmega328p";
    const uint32_t freq = 1000000;
    int debug = 0;
    int verbose = 0;

    for (int i = 1; i < argc; i++)
    {
        if (endswith(argv[i], ".hex"))
            strncpy(boot_path, argv[i], sizeof(boot_path));
        else if (!strcmp(argv[i], "-d"))
            debug++;
        else if (!strcmp(argv[i], "-v"))
            verbose++;
        else
        {
            fprintf(stderr, "%s: invalid argument %s\n", argv[0], argv[i]);
            exit(1);
        }
    }

    uint8_t *boot = read_ihex_file(boot_path, &boot_size, &boot_base);
    if (!boot)
    {
        fprintf(stderr, "%s: Unable to load %s\n", argv[0], boot_path);
        exit(1);
    }
    printf("%s bootloader 0x%05x: %d bytes\n", mmcu, boot_base, boot_size);

    avr_t *avr = avr_make_mcu_by_name(mmcu);
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

    snprintf(flash_data.avr_flash_path, sizeof(flash_data.avr_flash_path),
             "simduino_%s_flash.bin", mmcu);
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
    avr->log = 1 + verbose;

    // even if not setup at startup, activate gdb if crashing
    avr->gdb_port = 1234;
    if (debug)
    {
        avr->state = cpu_Stopped;
        avr_gdb_init(avr);
    }

    uart_pty_init(avr, &uart_pty);
    uart_pty_connect(&uart_pty, '0');

    while (1)
    {
        int state = avr_run(avr);
        if (state == cpu_Done || state == cpu_Crashed)
            break;
    }
}
