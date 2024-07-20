#include "led-counter.h"
#include "util.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rotary-encoder.h"

volatile unsigned long last_trigger = 0;

void cw_rotation_cb(void)
{
    increment_counter();
}

void ccw_rotation_cb(void)
{
    decrement_counter();
}

void single_button_press_cb(void)
{
    reset_led_counter();
}

void double_button_press_cb(void)
{
    reset_led_counter();
}

void long_button_press_cb(void)
{
    reset_led_counter();
}


int main()
{
    init_led_counter();

    init_rotary_encoder(cw_rotation_cb, ccw_rotation_cb, single_button_press_cb, double_button_press_cb, long_button_press_cb);
    increment_counter();
    sei();

    while (true)
    {
        if ((PIND & bit(SW_PIN)) == 0)
        {
            reset_led_counter();
        }
    }

    return 0;
}
