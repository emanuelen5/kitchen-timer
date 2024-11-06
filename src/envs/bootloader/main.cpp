#include <avr/io.h>
#include <util/delay.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/crc16.h>
#include "led-counter.h"

#if F_CPU != 1000000UL
#error The library can only handle a CPU frequency of 1MHz at the moment
#endif

void write_page(const uint8_t page_offset, const uint8_t *program_buffer)
{
    const uint16_t page_address = page_offset * SPM_PAGESIZE;
    boot_page_erase(page_address);
    boot_spm_busy_wait();

    const uint8_t word_size = 2;
    for (uint16_t byte_offset = 0; byte_offset < SPM_PAGESIZE; byte_offset += word_size)
    {
        uint16_t word = 0;
        word = *program_buffer++;
        word |= ((uint16_t)(*program_buffer++)) << 8;
        boot_page_fill(page_address + byte_offset, word);
    }

    boot_page_write(page_address);
    boot_spm_busy_wait();
}

static uint8_t sreg_last_state = 0;

void prepare_self_program(void)
{
    sreg_last_state = SREG;
    cli();
    eeprom_busy_wait();
}

void finalize_self_program(void)
{
    SREG = sreg_last_state;

    // Re-enable RWW-section. We need this to be able to jump back
    // to the application after bootloading.
    boot_rww_enable();
    SREG = sreg_last_state;
}

static inline void
jump_to_start_of_program_and_exit_bootloader(void)
{
    asm("jmp 0");
}

enum
{
    ok = 0x00,
    timeout = 0x05,
};

void UART_send(uint8_t data)
{
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = data;
}

int UART_receive(uint8_t *data)
{
    uint16_t timeout_counter = 2000;
    while (timeout_counter--)
    {
        if (UCSR0A & (1 << RXC0))
        {
            *data = UDR0;
            return 0; // Successful reception
        }
        _delay_ms(1);
    }
    return timeout;
}

typedef struct
{
    uint8_t command;
    uint8_t length;
    uint16_t checksum;
    union
    {
        struct
        {
            uint16_t page_offset;
        } read;
        struct
        {
            uint16_t page_offset;
            uint8_t data[SPM_PAGESIZE];
        } write;
    } data;
} packet_t;

void state_machine(void)
{
    enum
    {
        STATE_WAIT_FOR_PROGRAMMER,
        STATE_START,
        STATE_COMMAND,
        STATE_DATA,
        STATE_CHECKSUM1,
        STATE_CHECKSUM2,
    } state = STATE_START;

    enum
    {
        COMMAND_WRITE_PAGE
    };

    enum
    {
        START_BYTE = 0x03,
    };

    uint8_t received_byte;
    uint16_t calculated_checksum;

    packet_t packet;
    uint8_t data_index = 0;
    uint8_t program_data[SPM_PAGESIZE] = {0};

    while (1)
    {
        switch (state)
        {
        case STATE_WAIT_FOR_PROGRAMMER:
            set_counter(state);
            if (UART_receive(&received_byte) == timeout)
                return;

            if (received_byte == START_BYTE)
            {
                calculated_checksum = 0;
                state = STATE_COMMAND; // Start byte received, move to next state
            }
            else
            {
                state = STATE_START;
            }
            break;
        case STATE_START:
            set_counter(state);
            if (UART_receive(&received_byte) == timeout)
                return;

            if (received_byte == START_BYTE)
            {
                calculated_checksum = 0;
                state = STATE_COMMAND; // Start byte received, move to next state
            }
            break;

        case STATE_COMMAND:
            set_counter(state);
            if (UART_receive(&received_byte) != ok)
                continue;

            packet.command = received_byte;
            if (packet.command == COMMAND_WRITE_PAGE)
            {
                state = STATE_DATA;
            }
            data_index = 0;
            break;

        case STATE_DATA:
            if (UART_receive(&received_byte) != ok)
                continue;

            increment_counter();
            packet.data.write.data[data_index++] = received_byte;
            if (data_index >= 128)
            {
                state = STATE_CHECKSUM1;
                packet.checksum = 0;
            }
            break;

        case STATE_CHECKSUM1:
            set_counter(state);
            if (UART_receive(&received_byte) != ok)
                continue;

            state = STATE_CHECKSUM2;
            packet.checksum = received_byte;
            break;
        case STATE_CHECKSUM2:
            set_counter(state);
            if (UART_receive(&received_byte) != ok)
                continue;

            packet.checksum |= (uint16_t)received_byte << 8;

            if (calculated_checksum != packet.checksum)
            {
                UART_send(timeout);
                UART_send(~calculated_checksum); // Send checksum of the failure byte
                state = STATE_START;
                continue;
            }

            if (packet.command == COMMAND_WRITE_PAGE)
            {
                write_page(packet.data.write.page_offset, program_data);
                UART_send(ok);
                UART_send(calculated_checksum); // Send checksum of the success byte
                state = STATE_START;
            }
            break;
        }
    }
}

int main(void)
{
    init_led_counter();
    set_counter(0x7);

    prepare_self_program();

    state_machine();

    finalize_self_program();
    jump_to_start_of_program_and_exit_bootloader();
}