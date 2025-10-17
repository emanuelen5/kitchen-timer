#ifndef POWER_SAVE_H
#define POWER_SAVE_H

#include <stdint.h>
#include "config.h"

enum PowerState
{
    normal,
    display_dimmed,
    display_off,
    deep_sleep,
};

enum PowerSaveEvent
{
    no_activity_last_second,
    nothing_running_last_second,
    activity,
};

class PowerSave
{
public:
    PowerSave() {}

    void init(uint8_t *brightness)
    {
        wake_brightness = brightness;
        timeout_s = dim_timeout;
        state = PowerState::normal;
    }

    void handle_event(PowerSaveEvent event);

private:
    // n-1 seconds
    static constexpr uint16_t dim_timeout = 29;
    static constexpr uint16_t off_timeout = 59;
    static constexpr uint16_t deep_sleep_timeout = 0;

    uint8_t *wake_brightness;
    uint16_t timeout_s;
    PowerState state;
};

#endif // POWER_SAVE_H
