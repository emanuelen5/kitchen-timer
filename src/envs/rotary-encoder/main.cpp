#include "led-counter.h"
#include "util.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rotary-encoder.h"

volatile unsigned long last_trigger = 0;

void cw_rotation(void)
{
    increment_counter();
}

void ccw_rotation(void)
{
    decrement_counter();
}

int main()
{
    init_led_counter();

    init_rotary_encoder(cw_rotation, ccw_rotation);
    increment_counter();

    while (true)
    {
        if ((PIND & bit(SW_PIN)) == 0)
        {
            reset_led_counter();
        }
    }

    return 0;
}
