#include "SPI.h"
#include "util.h"

#define CLK_PIN PB5
#define DATA_PIN PB3

void init_SPI(uint8_t cs_pin) {
    DDRB |= bit(PB3) | bit(PB5) | bit(cs_pin); // Set MOSI, SCK, and CS as output

    SPCR = bit(SPE) | bit(MSTR) | bit(SPR1); //Enable SPI, Set as Master, Prescaler: Oscillator Frequency/64

    PORTB |= bit(cs_pin); //start with slave pin not selected
}

void SPI_transmit_byte(uint8_t byte)
{
    SPDR = byte;
    while (!(SPSR & bit(SPIF)));
}
