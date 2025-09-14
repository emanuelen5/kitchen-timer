#include "util.h"
#include "led-counter.h"
#include "millis.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

int main()
{
    init_hw_led_counter();
    init_hw_millis();
    uint16_t t = millis();

    sei();

    while (true)
    {
        uint16_t current_millis = millis();
        if (current_millis - t > 1000)
        {
            increment_counter();
            t += 1000;
        }
    }
}
