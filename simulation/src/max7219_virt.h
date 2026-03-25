/*
 * max7219_virt.h
 *
 * Virtual MAX7219 LED matrix driver peripheral for simavr.
 * Simulates a chain of 4 MAX7219 devices driving a 2x2 arrangement
 * of 8x8 LED matrices (16x16 pixel display).
 *
 * The MAX7219 receives 16-bit commands via SPI (8-bit register + 8-bit data).
 * With 4 daisy-chained devices, each SPI frame is 8 bytes (4 x 16 bits).
 * Data is latched on the rising edge of CS (active low).
 */

#ifndef __MAX7219_VIRT_H__
#define __MAX7219_VIRT_H__

#include "sim_irq.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX7219_NUM_DEVICES   4
#define MAX7219_NUM_DIGITS    8
#define MAX7219_MAX_SPI_BYTES (MAX7219_NUM_DEVICES * 2) /* 8 bytes per frame */

/* MAX7219 register addresses */
#define MAX7219_REG_NOOP        0x00
#define MAX7219_REG_DIGIT0      0x01
#define MAX7219_REG_DIGIT7      0x08
#define MAX7219_REG_DECODE_MODE 0x09
#define MAX7219_REG_INTENSITY   0x0A
#define MAX7219_REG_SCAN_LIMIT  0x0B
#define MAX7219_REG_SHUTDOWN    0x0C
#define MAX7219_REG_DISPLAY_TEST 0x0F

enum {
    IRQ_MAX7219_SPI_BYTE_IN = 0,
    IRQ_MAX7219_CS_IN,
    IRQ_MAX7219_COUNT
};

typedef struct max7219_virt_t {
    avr_irq_t *irq;
    struct avr_t *avr;

    /* SPI receive buffer for current frame */
    uint8_t spi_buffer[MAX7219_MAX_SPI_BYTES];
    uint8_t spi_byte_count;
    uint8_t cs_state; /* 0 = active (low), 1 = inactive (high) */

    /* Per-device state */
    struct {
        uint8_t digits[MAX7219_NUM_DIGITS]; /* digit register data (row data) */
        uint8_t intensity;
        uint8_t scan_limit;
        uint8_t shutdown;    /* 0 = shutdown, 1 = normal */
        uint8_t decode_mode;
        uint8_t display_test;
    } device[MAX7219_NUM_DEVICES];

    /* Combined 16x16 pixel framebuffer for easy rendering */
    uint8_t framebuffer[16][16]; /* [y][x], 0 or 1 */
    uint8_t dirty; /* set when framebuffer needs re-rendering */
} max7219_virt_t;

void max7219_virt_init(struct avr_t *avr, max7219_virt_t *part);

/* Rebuild framebuffer from device digit registers */
void max7219_virt_update_framebuffer(max7219_virt_t *part);

#ifdef __cplusplus
}
#endif

#endif /* __MAX7219_VIRT_H__ */
