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
    init_SPI(9);
    while (1)
    {
        add_to_SPI_queue(0xAA);
        add_to_SPI_queue(0xAA);
        start_SPI_transfer();
        _delay_ms(1000);
    }


    return 0;
}
