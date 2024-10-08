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

    if(is_on)
    {
        row_value |= bit(bit_offset);
    }
    else
    {
        row_value &= ~bit(bit_offset);
    }

    matrix_buffer[device_index][row_index] = row_value;
}



static inline void matrix_update_all_for_row(uint8_t row)
{
    max72xx_cmd_t cmds[MAX72XX_NUM_DEVICES];

    for (uint8_t device = 0; device < MAX72XX_NUM_DEVICES; device++)
    {
        const uint8_t spi_buffer_index = MAX72XX_NUM_DEVICES - 1 - device; // We send the last one first since they are shifted in
        cmds[spi_buffer_index].reg = (max72xx_reg_t)(Max72XX_Digit0 + row);
        cmds[spi_buffer_index].data = matrix_buffer[device][row];
    }

    max72xx_send_commands(cmds, MAX72XX_NUM_DEVICES);
}

void matrix_update(void) {
    for (uint8_t row=0; row < ROW_COUNT; row++)
    {
        matrix_update_all_for_row(row);
    }
}

void matrix_clear(void) {
    for (uint8_t row = 0; row < ROW_COUNT; row++)
    {
        for (uint8_t device = 0; device < MAX72XX_NUM_DEVICES; device++)
        {
            matrix_buffer[device][row] = 0x00;
        }
    }
    matrix_update();
}
