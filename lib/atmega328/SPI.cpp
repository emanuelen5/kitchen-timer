#include "SPI.h"

#define CLK_PIN PB5
#define DATA_PIN PB3

void init_SPI(uint8_t cs_pin) {
    DDRB |= (1 << PB3) | (1 << PB5) | (1 << cs_pin); // Set MOSI, SCK, and CS as output

    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1); //Enable SPI, Set as Master, Prescaler: Oscillator Frequency/64

    PORTB |= (1 << cs_pin); //start with slave pin not selected
}

void SPI_transmit_byte(uint8_t byte)
{
    SPDR = byte;
    while (!(SPSR & (1 << SPIF)));
}
