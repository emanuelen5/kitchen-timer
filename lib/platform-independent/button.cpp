#include "button.h"

// If we include "millis.h" the LDF will include all of the atmega328 libraries,
// which won't compile on the native host. So instead we just declare that the
// function should exist at link time.
extern uint16_t millis(void);

Button::Button()
    : last_press_time(0), press_count(0), is_pressed(false) {}

void Button::press()
{
    if (is_pressed)
        return;

    is_pressed = true;
    press_count++;

    uint16_t now = millis();
    uint16_t press_interval = now - last_press_time;

    bool is_double_press = press_count == 2 && press_interval <= double_press_timeout_ms;
    if (is_double_press)
    {
        on_double_press();
        press_count = 0;
    }

    last_press_time = now;
}

void Button::release()
{
    if (!is_pressed)
        return;

    is_pressed = false;
    uint16_t now = millis();
    uint16_t press_duration = now - last_press_time;

    if (press_duration > long_press_threshold_ms)
    {
        on_long_press();
        press_count = 0;
    }
}

/* Assumptions:
 * - must be called every ms;
 * - needs exclusive access (on AVR => `cli()` before being called)
 */
void Button::service()
{
    if (is_pressed)
        return;

    uint16_t now = millis();
    uint16_t ms_since_press = now - last_press_time;
    if (ms_since_press <= double_press_timeout_ms)
        return;

    if (press_count == 1)
    {
        on_single_press();
        press_count = 0;
    }
    else if (press_count == 2)
    {
        on_double_press();
        press_count = 0;
    }
}