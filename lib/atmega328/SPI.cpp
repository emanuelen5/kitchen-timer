#include "SPI.h"
uint8_t message_length;
void init_SPI(uint8_t bytes)
{
    // The SS pin must be set as an output, otherwise the SPI HW block will
    // switch from Master to slave mode whenever SS is driven low. Source: SS
    // Pin Functionality, ch. 19.3, megaAVR data sheet, Micro Chip, 2020.
    const uint8_t ss_pin = PB2;
    DDRB |= bit(DATA_PIN) | bit(CLK_PIN) | bit(ss_pin) | bit(CS_PIN);
    deactivate_cs();

    message_length = bytes;
}

void SPI_transmit_byte(uint8_t byte)
{
    SPDR = byte;
    while (!(SPSR & bit(SPIF)))
        ;
}
