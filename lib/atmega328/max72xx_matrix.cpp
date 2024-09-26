#include "max72xx_matrix.h"
#include <util.h>

static uint8_t matrix_buffer[MATRIX_HEIGHT][MATRIX_WIDTH];

void matrix_init(void) {
    init_max72xx();
    matrix_clear();
}

void matrix_clear(void) {
    for (uint8_t row = 0; row < MATRIX_HEIGHT; row++) {
        for (uint8_t col = 0; col < MATRIX_WIDTH; col++) {
            matrix_buffer[row][col] = 0;
        }
    }
    matrix_update();
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

