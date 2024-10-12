#include "SPI.h"
#include <avr/interrupt.h>
#include "util.h"
#include "max72xx.h"

#define CLK_PIN PB5
#define DATA_PIN PB3

extern SPI_context_t spi_cmds;
extern void deactivate_cs(void);

void init_SPI(uint8_t cs_pin)
{
    DDRB |= bit(PB3) | bit(PB5) | bit(cs_pin); // Set MOSI, SCK, and CS as output

    SPCR = bit(SPE) | bit(MSTR) | bit(SPR1) | bit(SPIE); //Enable SPI, Set as Master, Prescaler: Oscillator Frequency/16, enable SPI interrupt
    //TODO: Adjust the prescaler once we get the real PCB.

    PORTB |= bit(cs_pin); //start with slave pin not selected
}

ISR(SPI_STC_vect)
{
    spi_cmds.index++;

    if (spi_cmds.index < spi_cmds.length)
    {
        if (spi_cmds.index % 2 == 0)
        {
            SPDR = spi_cmds.cmds[spi_cmds.index / 2].reg; 
        }
        else
        {
            SPDR = spi_cmds.cmds[spi_cmds.index / 2].data; 
        }
    }
    else
    {
        deactivate_cs();
    }
}

void SPI_transmit_byte(uint8_t byte)
{
    SPDR = byte;
}
