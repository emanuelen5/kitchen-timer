#include "SPI.h"

#define CLK_PIN PB5
#define DATA_PIN PB3


void init_SPI(uint8_t cs_pin) {
    DDRB |= (1 << PB3) | (1 << PB5) | (1 << cs_pin); // Set MOSI, SCK, and CS as output

    SPCR = (1 << SPE); //Enable SPI
    SPCR = (1 << MSTR); //Set as Master
    SPCR = (1 << SPR0); //Prescaler: Oscillator Frequency/16

    PORTB |= (1 << cs_pin); //start with slave pin not selected
}

void SPI_transmit_byte(uint8_t byte)
{
    SPDR = byte;
    while (!(SPSR & (1 << SPIF)));
}
