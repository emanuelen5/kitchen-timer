#include "rotary-encoder.h"
#include "millis.h"
#include "util.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static rotation_cb_t rotation;

static Button *button = nullptr;
// State is stored as two bits (old A and old B)
static uint8_t encoder_state = 0;

// init_hw_millis must be called before this
void init_hw_rotary_encoder(rotation_cb_t rotation_cb, Button &button_)
{
    DDRD &= 0;
    PORTD |= bit(SW_PIN) | bit(CH_A_PIN) | bit(CH_B_PIN);

    uint8_t sreg = SREG;
    cli();

    // Enable both INT0 and INT1 interrupts
    EIMSK |= bit(INT0) | bit(INT1);

    // Set ISR0 and ISR1 to trigger on ANY edge (both rising and falling)
    EICRA = bit(ISC00) | bit(ISC10);

    PCICR |= bit(PCIE2);   // Enable Pin Change Interrupt for pin bank D
    PCMSK2 |= bit(SW_PIN); // Set mask to look for SW_PIN

    // Initialize encoder state based on current pin positions
    encoder_state = 0;
    if (bit_is_set(PIND, CH_A_PIN))
        encoder_state |= 1;
    if (bit_is_set(PIND, CH_B_PIN))
        encoder_state |= 2;

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
    timestamp_index = (timestamp_index + 1) % encoder_rotation_interval_buffer_size;

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

// Common function to handle encoder state changes
static void handle_encoder_interrupt()
{
    const uint8_t bank = PIND; // Read all values in the same time instant, early
    // Create the state using the current pin states and the previous state
    uint8_t new_state = encoder_state & 0b11; // Get old state (bits 0-1)
    if (bit_is_set(bank, CH_A_PIN))
        new_state |= bit(2); // new A
    if (bit_is_set(bank, CH_B_PIN))
        new_state |= bit(3); // new B

    static int8_t subincrement = 0;

    // Lookup table as in https://github.com/PaulStoffregen/Encoder/blob/c7627dd2fea62b3585154e430f3c036653a5c745/Encoder.h#L170-L188
    switch (new_state)
    {
    case 1:
    case 7:
    case 8:
    case 14:
        subincrement--;
        break;
    case 2:
    case 4:
    case 11:
    case 13:
        subincrement++;
        break;
    case 3:
    case 12:
        subincrement -= 2;
        break;
    case 6:
    case 9:
        subincrement += 2;
        break;
    default:
        break;
    }

    if (subincrement >= 4)
    {
        subincrement -= 4;
        rotation(cw, get_rotation_speed(), button->get_is_pressed());
        button->switch_to_rotation();
    }
    else if (subincrement <= -4)
    {
        subincrement += 4;
        rotation(ccw, get_rotation_speed(), button->get_is_pressed());
        button->switch_to_rotation();
    }

    encoder_state = (new_state >> 2);
}

ISR(INT0_vect)
{
    handle_encoder_interrupt();
}

ISR(INT1_vect)
{
    handle_encoder_interrupt();
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
