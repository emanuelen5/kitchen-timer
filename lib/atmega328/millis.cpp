#include <avr/io.h>
#include <avr/interrupt.h>
#include "util/atomic.h"
#include "util.h"

#if F_CPU != 1000000UL
#error The library can only handle a CPU frequency of 1MHz at the moment
#endif

static volatile uint16_t timer0_millis = 0;

ISR(TIMER0_COMPA_vect)
{
    timer0_millis += 1;
}

uint16_t millis(void)
{
    uint16_t m;

    // disable interrupts while we read timer0_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to timer0_millis)
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        m = timer0_millis;
    }

    return m;
}

void init_millis(void)
{
    const uint8_t timer0_ticks_in_a_ms = 125;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        TCCR0A = bit(WGM01); // CTC mode (count up to OCR0A)
        TCCR0B = bit(CS01);  // clk_IO / 8 prescaler
        OCR0A = timer0_ticks_in_a_ms - 1;
        TIMSK0 = bit(OCIE0A); // Timer compare interrupt
    }
}

void deinit_millis(void)
{
    TCCR0A = 0;
    TCCR0B = 0;
    TIMSK0 = 0;
}
