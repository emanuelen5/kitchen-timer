#include "max72xx_matrix.h"

static uint8_t matrix_buffer[MATRIX_HEIGHT][MATRIX_WIDTH];

void matrix_init(void) {
    init_max72xx();
    matrix_clear();
}

void matrix_clear(void) {
    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
        for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
            matrix_buffer[y][x] = 0;
        }
    }
    matrix_update();
}

void matrix_set_pixel(uint8_t x, uint8_t y, bool on) {
    if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return;
    
    matrix_buffer[y][x] = on ? 1 : 0;
}

void matrix_update(void) {
    // Iterate through all 16 rows
    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
        for (uint8_t x_device = 0; x_device < 2; x_device++) { // Two columns of devices
            uint8_t row_data = 0;
            
            // Generate the byte for the current 8-pixel-wide row section (x_device = 0 or 1)
            for (uint8_t x = 0; x < 8; x++) {
                uint8_t actual_x = x + (x_device * 8); // Calculate the actual x-coordinate in the 16x16 matrix
                if (matrix_buffer[y][actual_x]) {
                    row_data |= (1 << x); // Set the appropriate bit for the x column
                }
            }
            
            // Select the correct MAX7219 device based on y (row) and x_device (left or right 8x8 block)
            uint8_t device = x_device + (y / 8) * 2;
            uint8_t row_index = y % 8;
            
            // Send the data to the appropriate device and row
            max72xx_write_byte(device, Max72XX_Digit0 + row_index, row_data);
        }
    }
}

