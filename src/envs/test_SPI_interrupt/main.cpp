#include <avr/io.h>
#include "SPI.h"
#include <util/delay.h>
#include "led-counter.h"


void increment_counter_cb(void)
{
    increment_counter();
}

int main()
{
    init_led_counter();
    init_SPI(increment_counter_cb);
    while (1)
    {
        activate_cs();
        SPI_transmit_byte(0xAA);
        _delay_ms(10000);
    }


    return 0;
}
