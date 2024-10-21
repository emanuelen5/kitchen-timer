#include "SPI.h"
#include "util.h"

void init_SPI(void)
{
    // The SS pin must be set as an output, otherwise the SPI HW block will
    // switch from Master to slave mode whenever SS is driven low. Source: SS
    // Pin Functionality, ch. 19.3, megaAVR data sheet, Micro Chip, 2020.
    const uint8_t ss_pin = PB2;
    DDRB |= bit(DATA_PIN) | bit(CLK_PIN) | bit(ss_pin) | bit(CS_PIN);
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
