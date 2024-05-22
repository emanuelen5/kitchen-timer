#include "util.h"
#include <util/delay.h>
#include <avr/io.h>

int main()
{
    DDRB |= bit(1);
    PORTB |= bit(1);

    while (true)
    {

        PORTB ^= bit(1);
        _delay_ms(1000);
    }
}
