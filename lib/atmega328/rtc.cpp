#include "rtc.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util.h>

void do_nothing(void) {}
static callback *a_second_has_passed = &do_nothing;

ISR(TIMER2_OVF_vect)
{
    a_second_has_passed();
}

void init_hw_timer2_to_1s_interrupt(callback *on_second_cb)
{
    a_second_has_passed = on_second_cb;
    // Timer2 clocked from TOSC1 / external crystal
    ASSR |= bit(AS2);

    TCNT2 = 0; // Reset the start value of timer2

    TCCR2A = 0;
    TCCR2B = bit(CS22) | bit(CS20); // 32kHz / 128 prescaler = 1 second

    // Note: We don't synchronize with the ASSR register here on purpose since
    // that delays the boot by up to 3 seconds.
    //
    // It's only necessary to sync with ASSR if you want to write to any of the
    // timer2 registers. We are only interested in "reading" (rather the
    // triggering of the overflow interrupt), so it's fine for us to skip that.

    // TIMER2_OVF_vect enable
    TIMSK2 = bit(TOIE2);
}
