#include "max72xx_matrix.h"
#include "max72xx.h"
#include <util/delay.h>

int main()
{
    init_hw_max72xx();

    while (true)
    {

        for (uint8_t x = 0; x < 16; x++)
        {
            for (uint8_t y = 0; y < 16; y++)
            {
                matrix_set_pixel(x, y, true);
                matrix_update();
            }
        }

        for (uint8_t x = 0; x < 16; x++)
        {
            for (uint8_t y = 0; y < 16; y++)
            {
                matrix_set_pixel(x, y, false);
                matrix_update();
            }
        }
    }
    return 0;
}
