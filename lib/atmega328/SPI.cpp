#include "SPI.h"
#include "util.h"

void init_SPI(void)
{
    DDRB |= bit(DATA_PIN) | bit(CLK_PIN) | bit(CS_PIN);
    deactivate_cs();

    SPCR = bit(SPE) | bit(MSTR) | bit(SPR1); // Enable SPI, Set as Master, Prescaler: Oscillator Frequency/16.
    // TODO: Adjust the prescaler once we get the real PCB.
}

void SPI_transmit_byte(uint8_t byte)
{
    SPDR = byte;
    while (!(SPSR & bit(SPIF)))
        ;
}
