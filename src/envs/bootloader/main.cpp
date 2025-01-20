#include <avr/io.h>
#include <util/delay.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/crc16.h>
#include "led-counter.h"
#include "millis.h"

#include "bootloader_sm.h"

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

void read_signature(uint8_t signature[3])
{
    signature[0] = boot_signature_byte_get(0x00);
    signature[1] = boot_signature_byte_get(0x01);
    signature[2] = boot_signature_byte_get(0x02);
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

static void UART_send(uint8_t data)
{
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = data;
}

static int UART_receive(uint8_t *data)
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
            return resp_timeout;
    }
}

int receive_and_checksum(uint8_t *byte, uint16_t *crc16)
{
    int status = UART_receive(byte);
    if (status == resp_ok)
        *crc16 = _crc16_update(*crc16, *byte);
    return status;
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

    run_state_machine();

    finalize_self_program();
    deinit_millis();
    use_application_interrupt_vectors();
    jump_to_start_of_program_and_exit_bootloader();
}