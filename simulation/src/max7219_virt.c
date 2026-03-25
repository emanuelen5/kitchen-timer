/*
 * max7219_virt.c
 *
 * Virtual MAX7219 LED matrix driver peripheral for simavr.
 * Simulates a chain of 4 MAX7219 devices.
 *
 * Device layout (2x2 matrix, 16x16 pixels):
 *
 *   Device 3 (top-left)     | Device 2 (top-right)
 *   pixels (0,0)-(7,7)      | pixels (8,0)-(15,7)
 *   ------------------------+------------------------
 *   Device 1 (bottom-left)  | Device 0 (bottom-right)
 *   pixels (0,8)-(7,15)     | pixels (8,8)-(15,15)
 *
 * Mapping from pixel_to_device_index in the firmware:
 *   device_index = 3 - ((y/8)*2 + (x/8))
 *
 * SPI chain order: first bytes shifted go to the last device (device 3),
 * last bytes shifted stay at the first device (device 0).
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sim_avr.h"
#include "max7219_virt.h"

/*
 * Called when a SPI byte is output from the AVR.
 * We accumulate bytes until the CS line goes high (latch).
 */
static void
max7219_spi_in_hook(struct avr_irq_t *irq, uint32_t value, void *param)
{
    (void)irq;
    max7219_virt_t *part = (max7219_virt_t *)param;

    if (part->cs_state != 0) {
        /* CS is not active, ignore stray SPI data */
        return;
    }

    if (part->spi_byte_count < MAX7219_MAX_SPI_BYTES) {
        part->spi_buffer[part->spi_byte_count++] = (uint8_t)(value & 0xFF);
    }
}

/*
 * Process a command for a single MAX7219 device.
 */
static void
max7219_process_command(max7219_virt_t *part, uint8_t device_idx,
                        uint8_t reg, uint8_t data)
{
    if (device_idx >= MAX7219_NUM_DEVICES)
        return;

    if (reg >= MAX7219_REG_DIGIT0 && reg <= MAX7219_REG_DIGIT7) {
        uint8_t digit = reg - MAX7219_REG_DIGIT0;
        part->device[device_idx].digits[digit] = data;
        part->dirty = 1;
    } else {
        switch (reg) {
        case MAX7219_REG_NOOP:
            /* No operation */
            break;
        case MAX7219_REG_DECODE_MODE:
            part->device[device_idx].decode_mode = data;
            break;
        case MAX7219_REG_INTENSITY:
            part->device[device_idx].intensity = data;
            break;
        case MAX7219_REG_SCAN_LIMIT:
            part->device[device_idx].scan_limit = data;
            break;
        case MAX7219_REG_SHUTDOWN:
            part->device[device_idx].shutdown = data;
            part->dirty = 1;
            break;
        case MAX7219_REG_DISPLAY_TEST:
            part->device[device_idx].display_test = data;
            part->dirty = 1;
            break;
        default:
            break;
        }
    }
}

/*
 * Called when the CS pin changes state.
 * On the rising edge (low -> high), we latch the accumulated SPI data
 * and process the commands.
 */
static void
max7219_cs_hook(struct avr_irq_t *irq, uint32_t value, void *param)
{
    (void)irq;
    max7219_virt_t *part = (max7219_virt_t *)param;
    uint8_t new_cs = value ? 1 : 0;

    if (part->cs_state == 0 && new_cs == 1) {
        /* Rising edge: latch data */
        /*
         * SPI byte order in the buffer:
         *   Bytes [0,1]   -> shifted first -> ends at device N-1 (furthest)
         *   Bytes [2,3]   -> device N-2
         *   ...
         *   Bytes [6,7]   -> shifted last -> stays at device 0 (closest)
         *
         * Each 16-bit command: [register_byte, data_byte]
         */
        uint8_t num_commands = part->spi_byte_count / 2;
        for (uint8_t i = 0; i < num_commands && i < MAX7219_NUM_DEVICES; i++) {
            uint8_t reg  = part->spi_buffer[i * 2];
            uint8_t data = part->spi_buffer[i * 2 + 1];
            /*
             * First pair (i=0) goes to the last device in chain.
             * Last pair goes to device 0.
             */
            uint8_t device_idx = (num_commands - 1) - i;
            max7219_process_command(part, device_idx, reg, data);
        }

        part->spi_byte_count = 0;

        if (part->dirty) {
            max7219_virt_update_framebuffer(part);
        }
    }

    part->cs_state = new_cs;

    if (new_cs == 0) {
        /* CS went low: start of new frame */
        part->spi_byte_count = 0;
    }
}

/*
 * Rebuild the 16x16 framebuffer from the per-device digit registers.
 *
 * Device layout (from firmware pixel_to_device_index):
 *   device = 3 - ((y/8)*2 + (x/8))
 *
 *   Device 3: top-left     (x=0..7,  y=0..7)
 *   Device 2: top-right    (x=8..15, y=0..7)
 *   Device 1: bottom-left  (x=0..7,  y=8..15)
 *   Device 0: bottom-right (x=8..15, y=8..15)
 *
 * Within each device, from the firmware:
 *   row_index = 7 - (y % 8)    (pixel_to_device_row)
 *   bit       = x % 8          (pixel_to_bit)
 */
void
max7219_virt_update_framebuffer(max7219_virt_t *part)
{
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            uint8_t device_idx = 3 - ((y / 8) * 2 + (x / 8));
            uint8_t row_index = 7 - (y % 8);
            uint8_t bit_offset = x % 8;

            uint8_t row_data = part->device[device_idx].digits[row_index];
            uint8_t is_on = (row_data >> bit_offset) & 1;

            /* If device is in shutdown mode, all pixels off */
            if (part->device[device_idx].shutdown == 0)
                is_on = 0;

            /* Display test mode: all pixels on */
            if (part->device[device_idx].display_test)
                is_on = 1;

            part->framebuffer[y][x] = is_on;
        }
    }
    part->dirty = 0;
}

static const char *irq_names[IRQ_MAX7219_COUNT] = {
    [IRQ_MAX7219_SPI_BYTE_IN] = "8<max7219.spi_in",
    [IRQ_MAX7219_CS_IN]       = "<max7219.cs",
};

void
max7219_virt_init(avr_t *avr, max7219_virt_t *part)
{
    memset(part, 0, sizeof(*part));
    part->avr = avr;
    part->cs_state = 1; /* CS starts inactive (high) */
    part->dirty = 1;

    /* Default device state: shutdown mode */
    for (int i = 0; i < MAX7219_NUM_DEVICES; i++) {
        part->device[i].shutdown = 0;
        part->device[i].intensity = 0;
        part->device[i].scan_limit = 0;
        part->device[i].decode_mode = 0;
        part->device[i].display_test = 0;
        memset(part->device[i].digits, 0, MAX7219_NUM_DIGITS);
    }

    part->irq = avr_alloc_irq(&avr->irq_pool, 0, IRQ_MAX7219_COUNT, irq_names);

    avr_irq_register_notify(part->irq + IRQ_MAX7219_SPI_BYTE_IN,
                            max7219_spi_in_hook, part);
    avr_irq_register_notify(part->irq + IRQ_MAX7219_CS_IN,
                            max7219_cs_hook, part);
}
