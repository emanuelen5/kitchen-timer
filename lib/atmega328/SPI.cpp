#include "SPI.h"
#include <avr/interrupt.h>
#include "util/atomic.h"

static uint8_queue_t SPI_queue = {};
static const uint8_t SPI_queue_size = 9; // 64 bytes (2 bytes x 8 rows per device x 4 devices)
static uint8_t SPI_queue_buffer[SPI_queue_size + 1];

uint8_t message_length;
uint8_t bytes_transfered_counter;

static void inline deactivate_cs(void)
{
    PORTB |= bit(CS_PIN);
}

static void inline activate_cs(void)
{
    PORTB &= ~bit(CS_PIN);
}

void init_SPI(uint8_t bytes)
{
    // The SS pin must be set as an output, otherwise the SPI HW block will
    // switch from Master to slave mode whenever SS is driven low. Source: SS
    // Pin Functionality, ch. 19.3, megaAVR data sheet, Micro Chip, 2020.
    const uint8_t ss_pin = PB2;
    DDRB |= bit(DATA_PIN) | bit(CLK_PIN) | bit(ss_pin) | bit(CS_PIN);
    deactivate_cs();

    // Enable SPI, Master, set clock rate (fosc/16)
    SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR1);
    SPSR |= (1 << SPI2X);  // Double SPI speed if necessary (for 1 MHz)
    
    // Enable SPI interrupt
    SPCR |= (1 << SPIE);

    sei();
    
    message_length = bytes;
    init_queue(&SPI_queue, SPI_queue_buffer, SPI_queue_size);
}

void SPI_transmit_byte(uint8_t byte)
{
    SPDR = byte;
}

void add_to_SPI_queue(uint8_t value)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        add_to_queue(&SPI_queue, value);
    }
}

static bool is_SPI_transfer_ongoing()
{
    return (PORTB & bit(CS_PIN)) == 0;
}

void start_SPI_transfer()
{
    while (is_SPI_transfer_ongoing())
    {
    }

    activate_cs();
    dequeue_return_t transmition_starter;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        transmition_starter = dequeue(&SPI_queue);
    }

    bytes_transfered_counter = 0;
    if (transmition_starter.is_valid)
    {
        SPI_transmit_byte(transmition_starter.value);
    }
}

ISR(SPI_STC_vect)
{
    bytes_transfered_counter++;
    if (bytes_transfered_counter < message_length)
    {
        dequeue_return_t result = dequeue(&SPI_queue);
        if (result.is_valid)
        {
            SPI_transmit_byte(result.value);
        }
    }
    else
    {
        deactivate_cs();
    }
}
