#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include "util.h"

#define CS_PIN PB0
#define CLK_PIN PB5
#define DATA_PIN PB3

void init_SPI(void);
void SPI_transmit_byte(uint8_t byte);

static void inline deactivate_cs(void)
{
    PORTB |= bit(CS_PIN);
}

static void inline activate_cs(void)
{
    PORTB &= ~bit(CS_PIN);
}

#endif // SPI_H
