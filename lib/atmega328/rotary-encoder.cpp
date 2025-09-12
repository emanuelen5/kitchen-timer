#include "rotary-encoder.h"
#include "millis.h"
#include "util.h"

#include <avr/io.h>
#include <avr/interrupt.h>

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


static const uint8_t encoder_rotation_interval_buffer_size = 4;
static uint32_t timestamp_buffer[encoder_rotation_interval_buffer_size];
static uint8_t timestamp_index = 0;
static const uint8_t fast_encoder_step_threshold = 30;
static rotation_speed_t get_rotation_speed()
{
    timestamp_buffer[timestamp_index] = millis();
    timestamp_index = (timestamp_index + 1 ) % encoder_rotation_interval_buffer_size;

    uint8_t oldest_index = timestamp_index;
    uint8_t newest_index = (timestamp_index + encoder_rotation_interval_buffer_size - 1) % encoder_rotation_interval_buffer_size;
    uint32_t total_time = timestamp_buffer[newest_index] - timestamp_buffer[oldest_index];
    uint32_t average_interval = total_time >> 2;

    if (average_interval < fast_encoder_step_threshold)
    {
        return fast;
    }
    else
    {
        return slow;
    }
}

static uint16_t last_trigger_INT1 = 0;
ISR(INT1_vect)
{
    uint8_t bank = PIND; // Read as early as possible after interrupt
    if (!should_retrigger_after_sw_debounce(&last_trigger_INT1))
        return;

    bool ch_a = bit_is_set(bank, CH_A_PIN);
    const rotation_dir_t dir = ch_a ? cw : ccw;

    rotation(dir, get_rotation_speed(), button->get_is_pressed());
    button->switch_to_rotation();
}

static const uint16_t long_press_duration_ms = 2000;
static const uint16_t double_press_duration_ms = 500;
static uint16_t last_trigger_PCINT1 = 0;
ISR(PCINT2_vect)
{
    const bool is_pressed = bit_is_clear(PIND, SW_PIN); // Read as early as possible after interrupt
    if (!should_retrigger_after_sw_debounce(&last_trigger_PCINT1))
        return;

    if (is_pressed)
        button->press();
    else
        button->release();
}
