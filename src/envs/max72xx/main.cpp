#include "max72xx.h"
#include <util/delay.h>

uint8_t pattern[8] = {
        0b01111110, // Row 0
        0b01000010, // Row 1
        0b01011010, // Row 2
        0b01011010, // Row 3
        0b01011010, // Row 4
        0b01000010, // Row 5
        0b01111110, // Row 6
        0b00000000  // Row 7
    };

int main()
{
    init_max72xx();
    max72xx_clear();

    while(true)
    {
        for (uint8_t i = 0; i < 8; i++) {
            max72xx_write_byte(Max72XX_Digit0 + i, pattern[i]);
        }
        _delay_ms(1000);
    }
}
