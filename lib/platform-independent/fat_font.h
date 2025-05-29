#ifndef FAT_FONT_H
#define FAT_FONT_H

#include <stdint.h>

#define FATFONT_WIDTH 6
#define FATFONT_HEIGHT 7

const uint8_t* get_char(char c);

#endif // FAT_FONT_H
