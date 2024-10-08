#include "max72xx_matrix.h"
#include <util/delay.h>

int main()
{
    matrix_init();

    while(true)
    {



        for (uint8_t x = 0; x < 16; x++)
        {
            for (uint8_t y = 0; y < 16; y++ )
            {
                matrix_set_pixel(x, y, true);
                matrix_update();
                _delay_ms(1000);
            }
        }

        for (uint8_t x = 0; x < 16; x++)
        {
            for (uint8_t y = 0; y < 16; y++ )
            {
                matrix_set_pixel(x, y, false);
                matrix_update();
                _delay_ms(1000);
            }
        }
    }
    return 0;
}
