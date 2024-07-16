#include "button.h"

// If we include "millis.h" the LDF will include all of the atmega328 libraries,
// which won't compile on the native host. So instead we just declare that the
// function should exist at link time.
extern uint16_t millis(void);

Button::Button()
    : last_press_time(0), press_count(0), is_pressed(false) {}

void Button::press()
{
    is_pressed = true;
    last_press_time = millis();
    press_count++;

    uint16_t now = millis();
    uint16_t pressDuration = now - last_press_time;

    if (press_count == 2 && pressDuration <= double_press_timeout_ms)
    {
        on_double_press();
        press_count = 0;
    }
}

void Button::release()
{
    if (!is_pressed)
        return;

    is_pressed = false;
    uint16_t now = millis();
    uint16_t pressDuration = now - last_press_time;

    if (pressDuration > long_press_threshold_ms)
    {
        on_long_press();
        press_count = 0;
    }
}

void Button::service()
{
    uint16_t now = millis();

    if (is_pressed)
        return;

    if ((now - last_press_time) <= double_press_timeout_ms)
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