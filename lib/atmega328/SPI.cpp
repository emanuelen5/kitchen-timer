#include "SPI.h"

static uint8_queue_t SPI_queue = {};
static const uint8_t SPI_queue_size = 9; //64 bytes (2 bytes x 8 rows per device x 4 devices)
static uint8_t SPI_queue_buffer[SPI_queue_size];

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
    init_queue(&SPI_queue, SPI_queue_buffer, SPI_queue_size);
}

void SPI_transmit_byte(uint8_t byte)
{
    SPDR = byte;
}

void add_to_SPI_queue(uint8_t value)
{
    cli();
    add_to_queue(&SPI_queue, value);
    sei();
}

dequeue_return_t dequeue_from_SPI_queue(void)
{
    dequeue_return_t return_value = dequeue(&SPI_queue);
    return return_value;
}

}
