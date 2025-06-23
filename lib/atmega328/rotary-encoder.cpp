#include "rotary-encoder.h"
#include "millis.h"
#include "util.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static rotation_cb_t rotation;

static Button *button = nullptr;

void init_rotary_encoder(rotation_cb_t rotation_cb, Button &button_)
{
    init_millis();
    DDRD &= 0;
    PORTD |= bit(SW_PIN) | bit(CH_A_PIN) | bit(CH_B_PIN);

    uint8_t sreg = SREG;
    cli();
    EIMSK |= bit(INT1);                // Interrupt enable INT1
    EICRA |= bit(ISC11) | bit(ISC10); // rising edge interrupt on INT1

    PCICR |= bit(PCIE2);   // Enable Pin Change Interrupt for pin bank D
    PCMSK2 |= bit(SW_PIN); // Set mask to look for SW_PIN
    SREG = sreg;

    button = &button_;

    rotation = rotation_cb;
}

static bool should_retrigger_after_sw_debounce(uint16_t *last_trigger)
{
    uint16_t t = millis();
    // A time window of 2 ms garantees that at least we get 1 ms of window
    // between the trigger and the stable state.
    if (t - *last_trigger >= 2)
    {
        *last_trigger = t;
        return true;
    }

    return false;
}

static uint16_t last_trigger_INT1 = 0;
ISR(INT1_vect)
{
    if (should_retrigger_after_sw_debounce(&last_trigger_INT1))
    {
        uint8_t bank = PIND; // Read all values in the same time instant
        bool ch_a = bit_is_set(bank, CH_A_PIN);
        const rotation_dir_t dir = ch_a ? cw : ccw;

        rotation(dir, button->get_is_pressed());
        button->cancel_pending_event();
    }
}

static const uint16_t long_press_duration_ms = 2000;
static const uint16_t double_press_duration_ms = 500;
static uint16_t last_trigger_PCINT1 = 0;
ISR(PCINT2_vect)
{
    if (should_retrigger_after_sw_debounce(&last_trigger_PCINT1))
    {
        if (bit_is_clear(PIND, SW_PIN))
        {
            button->press();
        }
        else
        {
            button->release();
        }
    }
}
