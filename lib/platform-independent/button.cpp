#include "button.h"

// If we include "millis.h" the LDF will include all of the atmega328 libraries,
// which won't compile on the native host. So instead we just declare that the
// function should exist at link time.
extern uint16_t millis(void);

Button::Button(void (*single_press_handler)() = nullptr,
               void (*double_press_handler)() = nullptr,
               void (*long_press_handler)() = nullptr)
    : last_press_time(0),
      press_count(0),
      is_pressed(false),
      on_single_press(single_press_handler),
      on_double_press(double_press_handler),
      on_long_press(long_press_handler) {}

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
        invoke_double_press();
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

    if (press_count == 1)
    {
        if (press_duration > long_press_threshold_ms)
            invoke_long_press();
        else if (press_duration > double_press_timeout_ms)
            invoke_single_press();
    }
}

void Button::service()
{
    uint16_t now = millis();
    uint16_t ms_since_press = now - last_press_time;

    if (press_count == 1 && is_pressed && ms_since_press > long_press_threshold_ms)
    {
        invoke_long_press();
        return;
    }

    if (press_count == 1 && !is_pressed && ms_since_press > double_press_timeout_ms)
    {
        invoke_single_press();
    }
}

void Button::switch_to_rotation()
{
    uint16_t now = millis();
    uint16_t ms_since_press = now - last_press_time;
    if (press_count == 1 && ms_since_press > press_to_rotation_timeout_ms)
    {
        invoke_single_press();
    }
    press_count = 0;
}

void Button::invoke_single_press()
{
    press_count = 0;
    if (on_single_press)
        on_single_press();
};

void Button::invoke_double_press()
{
    press_count = 0;
    if (on_double_press)
        on_double_press();
};

void Button::invoke_long_press()
{
    press_count = 0;
    if (on_long_press)
        on_long_press();
};