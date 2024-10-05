#ifndef LIB_MAX72XX_MATRIX_H
#define LIB_MAX72XX_MATRIX_H

#include "max72xx.h"

#define MATRIX_COL_WIDTH 16
#define MATRIX_ROW_HEIGHT 16
#define ROW_COUNT 8

void matrix_init(void);
void matrix_set_pixel(uint8_t x, uint8_t y, bool is_on);
void matrix_update(void);
void matrix_clear(void);

#endif // LIB_MAX72XX_MATRIX_H
