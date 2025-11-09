#include <stdint.h>
#include "fat_font.h"

// Each byte in the characters uses the 6 least significant bits.

static const uint8_t font_table[10][FATFONT_HEIGHT] = {
    {
        0b011110,
        0b110011,
        0b110111,
        0b111111,
        0b111011,
        0b110011,
        0b011110
    },
    {
        0b001100,
        0b011100,
        0b001100,
        0b001100,
        0b001100,
        0b001100,
        0b111111
    },
    {
        0b011110,
        0b110011,
        0b000011,
        0b001110,
        0b011000,
        0b110000,
        0b111111
    },
    {
        0b011110,
        0b110011,
        0b000011,
        0b001110,
        0b000011,
        0b110011,
        0b011110
    },
    {
        0b000111,
        0b001111,
        0b011011,
        0b110011,
        0b111111,
        0b000011,
        0b000011
    },
    {
        0b111111,
        0b110000,
        0b111110,
        0b000011,
        0b000011,
        0b110011,
        0b011110
    },
    {
        0b011110,
        0b110011,
        0b110000,
        0b111110,
        0b110011,
        0b110011,
        0b011110
    },
    {
        0b111111,
        0b000011,
        0b000011,
        0b000110,
        0b001100,
        0b001100,
        0b001100
    },
    {
        0b011110,
        0b110011,
        0b110011,
        0b011110,
        0b110011,
        0b110011,
        0b011110
    },
    {
        0b011110,
        0b110011,
        0b110011,
        0b011111,
        0b000011,
        0b110011,
        0b011110
    }
};

static const uint8_t char_h[FATFONT_HEIGHT] = {
    0b000000,
    0b110000,
    0b110000,
    0b111110,
    0b110011,
    0b110011,
    0b110011
};

static const uint8_t char_v[FATFONT_HEIGHT] = {
    0b110110,
    0b110110,
    0b110110,
    0b110110,
    0b110110,
    0b011100,
    0b001000
};

const uint8_t* get_bitmap(char c)
{
    if (c >= '0' && c <= '9')
    {
        return font_table[c - '0'];
    }
    switch (c)
    {
        case 'h':
        case 'H':
            return char_h;
        case 'v':
        case 'V':
            return char_v;
    }
    return nullptr;
}
