#ifndef LIB_MAX72XX_MATRIX_H
#define LIB_MAX72XX_MATRIX_H

#include "max72xx.h"

#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16

void matrix_init(void);
void matrix_clear(void);
void matrix_set_pixel(uint8_t col, uint8_t row, bool on);
void matrix_update(void);

#endif // LIB_MAX72XX_MATRIX_H
