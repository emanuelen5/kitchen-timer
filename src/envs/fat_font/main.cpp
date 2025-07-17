#include <util/delay.h>
#include "max72xx_matrix.h"
#include "fat_font.h"

void draw_digit(uint8_t digit, uint8_t x_offset, uint8_t y_offset);

int main()
{
    matrix_init();

    while(true)
    {
        for(uint8_t number = 0; number < 10; number++)
        {
            draw_digit(number, 2, 8);
            matrix_update();
            _delay_ms(1000);
        }
    }
    
    return 0;
}

void draw_digit(uint8_t digit, uint8_t x_offset, uint8_t y_offset)
{
    const uint8_t* ptr_digit = get_char('0' + digit);
    
    for (uint8_t row = 0; row < 7; row++)
    {
        for (uint8_t col = 0; col < 6; col++)
        {
            bool is_on = ptr_digit[row] & (1 << (5 - col));  // 6-bit wide
            matrix_set_pixel(x_offset + col, y_offset + row, is_on);
        }
    }
}
