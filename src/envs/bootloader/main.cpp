#include <avr/io.h>
#include <util/delay.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
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

int main(void)
{
    init_led_counter();
    set_counter(0x7);

    prepare_self_program();

    uint8_t program_data[SPM_PAGESIZE] = {0};
    const uint8_t page_count = 1;

    for (uint8_t page_offset = 0; page_offset < page_count; page_offset++)
    {
        increment_counter();
        write_page(page_offset, program_data);
    }

    finalize_self_program();
    jump_to_start_of_program_and_exit_bootloader();
}