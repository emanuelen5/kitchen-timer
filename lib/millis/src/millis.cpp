#include <avr/io.h>
#include <avr/interrupt.h>
#include "util.h"

#if F_CPU != 1000000UL
#error The library can only handle a CPU frequency of 1MHz at the moment
#endif

static volatile unsigned long timer0_millis = 0;

ISR(TIMER0_COMPA_vect)
{
    timer0_millis += 1;
}

unsigned long millis(void)
{
    unsigned long m;
    uint8_t oldSREG = SREG;

    // disable interrupts while we read timer0_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to timer0_millis)
    cli();
    m = timer0_millis;
    SREG = oldSREG;

    return m;
}

void init_millis(void)
{
    const uint8_t timer0_ticks_in_a_ms = 125;

    uint8_t sreg = SREG;
    cli();

    TCCR0A = bit(WGM01); // CTC mode (count up to OCR0A)
    TCCR0B = bit(CS01);  // clk_IO / 8 prescaler
    OCR0A = timer0_ticks_in_a_ms - 1;
    TIMSK0 = bit(OCIE0A); // Timer compare interrupt

    SREG = sreg;
}
