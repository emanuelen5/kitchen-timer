#include <Arduino.h>
#include "rotary-encoder.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static event_cb_t cw_rotation;
static event_cb_t ccw_rotation;
static event_cb_t button_press;

void init_rotary_encoder(event_cb_t cw_rotation_cb, event_cb_t ccw_rotation_cb, event_cb_t button_press_cb)
{
    DDRD &= 0;
    PORTD |= bit(SW_PIN) | bit(CLK_PIN) | bit(DT_PIN);

    cli();
    EIMSK |= bit(INT0);                // Interrupt enable INT0
    EICRA |= ~bit(ISC01) | bit(ISC00); // any change interrupt on INT0

    PCICR |= bit(PCIE2);               // Enable Pin Change Interrupt for pin bank D
    PCMSK2 |= bit(SW_PIN);             // Set mask to look for SW_PIN
    sei();

    button_press = button_press_cb;
    cw_rotation = cw_rotation_cb;
    ccw_rotation = ccw_rotation_cb;
}

static bool should_retrigger_after_sw_debounce(unsigned long *last_trigger)
{
    unsigned long t = millis();
    // A time window of 2 ms garantees that at least we get 1 ms of window
    // between the trigger and the stable state.
    if (t - *last_trigger >= 2)
    {
        *last_trigger = t;
        return true;
    }

    return false;
}

unsigned long last_trigger_INT0 = 0;
ISR(INT0_vect)
{
    if (should_retrigger_after_sw_debounce(&last_trigger_INT0))
    {
        uint8_t bank = PIND; // Read all values in the same time instant
        bool clk = bit_is_set(bank, CLK_PIN);
        bool dt = bit_is_set(bank, DT_PIN);
        if (clk == dt)
            ccw_rotation();
        else
            cw_rotation();
    }
}

unsigned long last_trigger_PCINT0 = 0;
ISR(PCINT2_vect)
{
    if (should_retrigger_after_sw_debounce(&last_trigger_PCINT0))
    {
        if (bit_is_set(PIND, SW_PIN))
        {
            button_press();
        }
    }
}
