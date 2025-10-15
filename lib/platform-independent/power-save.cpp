#include "power-save.h"

// HW specific functions to be provided by the integrator
void max72xx_set_intensity(uint8_t intensity);
void max72xx_display_on(void);
void max72xx_display_off(void);
void enter_deep_sleep(void);

void PowerSave::handle_event(PowerSaveEvent event)
{
    switch (state)
    {
    case PowerState::normal:
        if (event == activity)
        {
            timeout_s = dim_timeout;
        }
        else if (event == no_activity_last_second || event == nothing_running_last_second)
        {
            if (timeout_s == 0)
            {
                max72xx_set_intensity(0);
                state = PowerState::display_dimmed;
                timeout_s = off_timeout;
            }
            else
                timeout_s--;
        }
        break;
    case PowerState::display_dimmed:
        if (event == activity)
        {
            max72xx_set_intensity(*wake_brightness);
            state = PowerState::normal;
            timeout_s = dim_timeout;
        }
        else if (event == nothing_running_last_second)
        {
            if (timeout_s == 0)
            {
                max72xx_display_off();
                state = PowerState::display_off;
                timeout_s = deep_sleep_timeout;
            }
            else
                timeout_s--;
        }
        break;
    case PowerState::display_off:
        if (event == PowerSaveEvent::activity)
        {
            max72xx_display_on();
            max72xx_set_intensity(*wake_brightness);
            state = PowerState::normal;
            timeout_s = dim_timeout;
        }
        else if (event == nothing_running_last_second)
        {
            if (timeout_s == 0)
            {
                enter_deep_sleep();
                state = PowerState::deep_sleep;
            }
            else
                timeout_s--;
        }
        break;
    case PowerState::deep_sleep:
        if (event == activity)
        {
            // uC has already woken up by itself. We only need to restore the
            // application state and external hardware
            max72xx_display_on();
            max72xx_set_intensity(*wake_brightness);
            state = PowerState::normal;
            timeout_s = dim_timeout;
        }
        else
        {
            // This should not happen. We should have turned off the timer
            // interrupt when in deep sleep. Otherwise we get unnecessary
            // interrupts

            // TODO: log this somehow
        }
    }
}
