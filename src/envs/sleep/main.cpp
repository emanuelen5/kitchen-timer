#include <Arduino.h>
#include "led-counter.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <util/delay.h>

static const uint8_t power_save = bit(SM1) | bit(SM0);

ISR(TIMER2_OVF_vect)
{
    increment_counter();
}

void init_timer2_to_1s_interrupt(void)
{
    // Timer2 clocked from TOSC1 / external crystal
    ASSR |= bit(AS2);

    TCNT2 = 0; // Reset the start value of timer2

    TCCR2B = bit(CS22); // 32kHz / 128 prescaler = 1 second

    // Wait for the registers to update
    while (ASSR & 0x1F)
    {
    }

    // TIMER2_OVF_vect enable
    TIMSK2 = bit(TOIE2);
}

void setup()
{
    init_led_counter();
    init_timer2_to_1s_interrupt();
    set_sleep_mode(power_save);
    sei();
}

void loop()
{
    sleep();
    decrement_counter();
}
