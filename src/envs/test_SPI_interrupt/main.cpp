#include <avr/io.h>
#include "SPI.h"
#include <util/delay.h>
#include "led-counter.h"


int main()
{
    init_hw_led_counter();
    init_hw_SPI(9);
    while (1)
    {
        add_to_SPI_queue(0xAA);
        add_to_SPI_queue(0xAA);
        start_SPI_transfer();
        _delay_ms(1000);
    }


    return 0;
}
