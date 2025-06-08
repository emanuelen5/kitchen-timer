#include <avr/io.h>
#include <util/delay.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/crc16.h>
#include "led-counter.h"
#define BAUD 9600
#include <util/setbaud.h>

#include "bootloader_sm.h"

#if F_CPU != 1000000UL
#error The library can only handle a CPU frequency of 1MHz at the moment
#endif

#define bit(x) (1 << (x))

static uint8_t sreg_last_state = 0;

void prepare_self_program(void)
{
    cli();
    eeprom_busy_wait();
}

void write_page(const uint16_t page_offset, const uint8_t *program_buffer)
{
    prepare_self_program();

    const uint16_t page_address = page_offset * SPM_PAGESIZE;
    // 2 pages are erased at once
    if (page_offset == 0)
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

void read_page(const uint16_t page_offset, uint8_t *program_buffer)
{
    prepare_self_program();

    const uint16_t page_address = page_offset * SPM_PAGESIZE;
    for (uint16_t byte_offset = 0; byte_offset < SPM_PAGESIZE; byte_offset++)
    {
        program_buffer[byte_offset] = pgm_read_byte(page_address + byte_offset);
    }

    SREG = sreg_last_state;
}

void read_signature(uint8_t signature[3])
{
    prepare_self_program();
    signature[0] = boot_signature_byte_get(0x00);
    signature[1] = boot_signature_byte_get(0x02);
    signature[2] = boot_signature_byte_get(0x04);
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

static void init_UART(void)
{
    // Set baud rate
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= bit(U2X0);
#else
    UCSR0A &= ~bit(U2X0);
#endif

    UCSR0B = bit(RXEN0) | bit(TXEN0);   // Enable rx/tx; and rx/tx interrupt
    UCSR0C = bit(UCSZ01) | bit(UCSZ00); // Set frame format: 8 data bits, 1 stop bit, no parity
}

void UART_send(uint8_t data)
{
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = data;
}

uint8_t UART_receive(void)
{
    while (!(UCSR0A & (1 << RXC0)))
        ;
    return UDR0;
}

int UART_receive_with_timeout(uint8_t *data)
{
    const uint8_t timeout = 1;
    uint32_t retries = 0x1ffffUL; // approximately 2 seconds
    while (true)
    {
        if (UCSR0A & (1 << RXC0))
        {
            *data = UDR0;
            return 0; // Successful reception
        }

        if (retries == 0)
            return timeout;

        retries--;
    }
}

uint16_t send_and_checksum(uint8_t byte, uint16_t crc16)
{
    UART_send(byte);
    return _crc16_update(crc16, byte);
}

void run_state_machine(void)
{
    state_machine_t sm = {};

    while (sm.state != STATE_EXIT)
    {
        step_state_machine(sm);
    }
}

int main(void)
{
    init_led_counter();
    init_UART();
    set_counter(0x7);

    run_state_machine();

    finalize_self_program();
    jump_to_start_of_program_and_exit_bootloader();
}