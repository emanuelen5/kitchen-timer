#include "max72xx_matrix.h"
#include <util.h>

static uint8_t matrix_buffer[MAX72XX_NUM_DEVICES][ROW_COUNT];

void matrix_init(void) {
    init_max72xx();
    matrix_clear();
}

static inline uint8_t pixel_to_device_index(uint8_t x, uint8_t y)
{
    return (y / 8)* 2 + (x / 8);
}

static inline uint8_t pixel_to_device_row(uint8_t y)
{
    return y % 8;
            
}

static inline uint8_t pixel_to_bit(uint8_t x)
{
    return x % 8;
}

void matrix_set_pixel(uint8_t x, uint8_t y, bool is_on)
{
    if (x >= MATRIX_COL_WIDTH || y >= MATRIX_ROW_HEIGHT)
        return;
    
    const uint8_t device_index = pixel_to_device_index(x, y);
    const uint8_t row_index = pixel_to_device_row(y);
    const uint8_t bit_offset = pixel_to_bit(x);

    uint8_t row_value = matrix_buffer[device_index][row_index];

    row_value &= ~bit(bit_offset);
    row_value |= (is_on ? bit(bit_offset) : 0);
    matrix_buffer[device_index][row_index] = row_value;
}

void matrix_set_pixel(uint8_t col, uint8_t row, bool on) {
    if (col >= MATRIX_WIDTH || row >= MATRIX_HEIGHT) return;
    
    matrix_buffer[row][col] = on ? 1 : 0;
}

void matrix_update(void) {
    for (uint8_t row = 0; row < MATRIX_HEIGHT; row++) {
        for (uint8_t col_device = 0; col_device < 2; col_device++) { // Two columns of devices
            uint8_t row_buffer = 0;
            
            for (uint8_t col = 0; col < 8; col++) {
                uint8_t column_position = col + (col_device * 8);
                if (matrix_buffer[row][column_position]) {
                    row_buffer |= bit(col);
                }
            }
            
            uint8_t device = col_device + (row / 8) * 2;
            uint8_t row_index = row % 8;
            
            max72xx_write_byte(device, Max72XX_Digit0 + row_index, row_buffer);
        }
    }
}

