#include "led-counter.h"
#include "util.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rotary-encoder.h"
#include "avr_button.h"

volatile unsigned long last_trigger = 0;

void rotation_cb(rotation_dir_t dir, rotation_speed_t speed, bool held_down)
{
    UNUSED held_down;
    UNUSED speed;
    if (dir == cw)
        increment_counter();
    else if (dir == ccw)
        decrement_counter();
}

void on_single_press(void)
{
    reset_led_counter();
}

void on_double_press(void)
{
    reset_led_counter();
}

void on_long_press(void)
{
    reset_led_counter();
}


int main()
{
    AvrButton button(&on_single_press, &on_double_press, &on_long_press);
    init_led_counter();

    init_rotary_encoder(rotation_cb, button);
    increment_counter();
    sei();

    while (true)
    {
        if ((PIND & bit(SW_PIN)) == 0)
        {
            reset_led_counter();
        }
        button.service();
    }

    return 0;
}
