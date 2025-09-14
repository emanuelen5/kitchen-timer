#include "led-counter.h"
#include "util.h"
#include "rtc.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <util/delay.h>

static const uint8_t power_save = bit(SM1) | bit(SM0);

void on_second_tick(void)
{
    increment_counter();
}

int main()
{
    init_hw_led_counter();
    init_hw_timer2_to_1s_interrupt(&on_second_tick);
    set_sleep_mode(power_save);
    sei();

    for (;;)
    {
        sleep_mode();
        decrement_counter();
    }

    return 0;
}
