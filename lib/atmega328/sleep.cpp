#include <avr/sleep.h>
#include <stdint.h>
#include "util.h"

static const uint8_t power_down = bit(SM1);

void enter_deep_sleep(void)
{
    set_sleep_mode(power_down);
    sleep_mode();
}
