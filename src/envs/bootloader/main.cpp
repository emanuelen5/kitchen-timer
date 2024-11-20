#include <avr/io.h>
#include <util/delay.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/crc16.h>
#include "led-counter.h"
#include "millis.h"
#include "util.h"

#if F_CPU != 1000000UL
#error The library can only handle a CPU frequency of 1MHz at the moment
#endif

static uint8_t sreg_last_state = 0;

void prepare_self_program(void)
{
    sreg_last_state = SREG;
    cli();
    eeprom_busy_wait();
}

void write_page(const uint8_t page_offset, const uint8_t *program_buffer)
{
    prepare_self_program();

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

    SREG = sreg_last_state;
}

void finalize_self_program(void)
{
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
    uint16_t start_time = millis();
    const uint16_t timeout_duration = 2000;
    while (true)
    {
        if (UCSR0A & (1 << RXC0))
        {
            *data = UDR0;
            return 0; // Successful reception
        }

        if ((uint16_t)(millis() - start_time) >= timeout_duration)
            return timeout;
    }
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
    } state = STATE_WAIT_FOR_PROGRAMMER;

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

            if (received_byte != START_BYTE)
            {
                state = STATE_START;
                continue;
            }
        case STATE_START:
            set_counter(state);
            if (received_byte == START_BYTE)
            {
                calculated_checksum = 0;
                state = STATE_COMMAND;
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

void use_bootloader_interrupt_vectors(void)
{
    asm volatile(
        "ldi r16, %[ivce_bit]       \n\t"
        "out %[mcucr], r16          \n\t"
        "ldi r16, %[ivsel_bit]      \n\t"
        "out %[mcucr], r16          \n\t"
        :
        : [ivce_bit] "M"(1 << IVCE),
          [ivsel_bit] "M"(1 << IVSEL),
          [mcucr] "I"(_SFR_IO_ADDR(MCUCR))
        : "r16");
}

void use_application_interrupt_vectors(void)
{
    asm volatile(
        "ldi r16, %[ivce_bit]       \n\t"
        "out %[mcucr], r16          \n\t"
        "ldi r16, %[ivsel_bit]      \n\t"
        "out %[mcucr], r16          \n\t"
        :
        : [ivce_bit] "M"(1 << IVCE),
          [ivsel_bit] "M"(0),
          [mcucr] "I"(_SFR_IO_ADDR(MCUCR))
        : "r16");
}

int main(void)
{
    use_bootloader_interrupt_vectors();
    init_millis();
    init_led_counter();
    set_counter(0x7);
    sei();

    state_machine();

    finalize_self_program();
    deinit_millis();
    use_application_interrupt_vectors();
    jump_to_start_of_program_and_exit_bootloader();
}