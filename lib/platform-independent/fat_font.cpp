#include <stdint.h>
#include "fat_font.h"

#if defined(__AVR__)
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

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

struct letter_t
{
    char character;
    uint8_t bitmap[FATFONT_HEIGHT];
};

static constexpr letter_t letter_table[] = {
    {'E', {
        0b111111,
        0b110000,
        0b110000,
        0b111110,
        0b110000,
        0b110000,
        0b111111
    }},
    {'H', {
        0b000000,
        0b110000,
        0b110000,
        0b111110,
        0b110011,
        0b110011,
        0b110011
    }},
    {'M', {
        0b110011,
        0b111011,
        0b111111,
        0b110011,
        0b110011,
        0b110011,
        0b110011
    }},
    {'O', {
        0b011110,
        0b110011,
        0b110011,
        0b110011,
        0b110011,
        0b110011,
        0b011110
    }},
    {'R', {
        0b111110,
        0b110011,
        0b110011,
        0b111110,
        0b110110,
        0b110011,
        0b110011
    }},
    {'S', {
        0b011110,
        0b110011,
        0b110000,
        0b011110,
        0b000011,
        0b110011,
        0b011110
    }},
    {'T', {
        0b111111,
        0b001100,
        0b001100,
        0b001100,
        0b001100,
        0b001100,
        0b001100
    }},
    {'U', {
        0b110011,
        0b110011,
        0b110011,
        0b110011,
        0b110011,
        0b110011,
        0b011110
    }},
    {'V', {
        0b110110,
        0b110110,
        0b110110,
        0b110110,
        0b110110,
        0b011100,
        0b001000
    }}
};

static constexpr uint8_t letter_count = sizeof(letter_table) / sizeof(letter_table[0]);

// Position of the letter in letter_table, or letter_count if there is no glyph for it
static constexpr uint8_t find_letter(char c, uint8_t position = 0)
{
    return position == letter_count               ? letter_count
           : letter_table[position].character == c ? position
                                                   : find_letter(c, position + 1);
}

static constexpr uint8_t letter_positions['Z' - 'A' + 1] = {
    find_letter('A'), find_letter('B'), find_letter('C'), find_letter('D'), find_letter('E'), find_letter('F'),
    find_letter('G'), find_letter('H'), find_letter('I'), find_letter('J'), find_letter('K'), find_letter('L'),
    find_letter('M'), find_letter('N'), find_letter('O'), find_letter('P'), find_letter('Q'), find_letter('R'),
    find_letter('S'), find_letter('T'), find_letter('U'), find_letter('V'), find_letter('W'), find_letter('X'),
    find_letter('Y'), find_letter('Z')
};

const uint8_t _icon_brightness[] PROGMEM = {
    0b00000000,0b00000000,
    0b00000000,0b10000000,
    0b00010000,0b10000100,
    0b00001000,0b00001000,
    0b00000001,0b11000000,
    0b00000011,0b11100000,
    0b00000111,0b11110000,
    0b00110111,0b11110110,
    0b00000111,0b11110000,
    0b00000011,0b11100000,
    0b00000001,0b11000000,
    0b00001000,0b00001000,
    0b00010000,0b10000100,
    0b00000000,0b10000000,
    0b00000000,0b00000000,
    0b00000000,0b00000000,
};

const uint8_t _icon_volume[] PROGMEM = {
    0b00000000,0b00000000,
    0b00000000,0b00000000,
    0b00000000,0b00000000,
    0b00000001,0b00100000,
    0b00000011,0b00110000,
    0b00000111,0b00011000,
    0b00111111,0b01001100,
    0b00111111,0b01100100,
    0b00111111,0b00100100,
    0b00111111,0b01100100,
    0b00111111,0b01001100,
    0b00000111,0b00011000,
    0b00000011,0b00110000,
    0b00000001,0b00100000,
    0b00000000,0b00000000,
    0b00000000,0b00000000,
};

const uint8_t _icon_battery[] PROGMEM = {
    0b00000000,0b00000000,
    0b00000000,0b00000000,
    0b00000000,0b00010000,
    0b00000000,0b00110000,
    0b00000000,0b01100000,
    0b00000000,0b11000000,
    0b00000001,0b10000000,
    0b00000011,0b11111000,
    0b00000111,0b11110000,
    0b00001111,0b11100000,
    0b00000000,0b11000000,
    0b00000001,0b10000000,
    0b00000011,0b00000000,
    0b00000110,0b00000000,
    0b00000100,0b00000000,
    0b00000000,0b00000000,
};

const uint8_t _icon_melody[] PROGMEM = {
    0b00000000,0b00000000,
    0b00000000,0b00000000,
    0b00000000,0b00001100,
    0b00000000,0b01111100,
    0b00000011,0b11111100,
    0b00000011,0b11000100,
    0b00000010,0b00000100,
    0b00000010,0b00000100,
    0b00000010,0b00000100,
    0b00000010,0b00000100,
    0b00000010,0b00000100,
    0b00001110,0b00011100,
    0b00011110,0b00111100,
    0b00011110,0b00111100,
    0b00001100,0b00011000,
    0b00000000,0b00000000,
};

const uint8_t _icon_snake[] PROGMEM = {
    0b00000000,0b00000000,
    0b00000001,0b10000000,
    0b00000011,0b11000000,
    0b00000111,0b11100000,
    0b00000110,0b01100000,
    0b00000110,0b01110000,
    0b00110110,0b01110110,
    0b01111110,0b01101111,
    0b01111100,0b11101100,
    0b00110000,0b11001100,
    0b00000001,0b11001100,
    0b00000001,0b11001100,
    0b00000000,0b11111100,
    0b00000000,0b01111000,
    0b00000000,0b00110000,
    0b00000000,0b00000000,
};

const uint8_t _icon_return[] PROGMEM= {
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000100, 0b00000000,
    0b00001100, 0b00000000,
    0b00011111, 0b11100000,
    0b00111111, 0b11110000,
    0b01111111, 0b11111000,
    0b00111111, 0b11111100,
    0b00011111, 0b11111100,
    0b00001100, 0b00111100,
    0b00000100, 0b00111000,
    0b00000000, 0b00111000,
    0b00000000, 0b01110000,
    0b00000000, 0b11100000,
    0b00000001, 0b11000000,
    0b00000000, 0b00000000,
};

const uint8_t* get_icon_bitmap(enum icons icon)
{
    switch (icon)
    {
        case icon_brightness:
            return _icon_brightness;
        case icon_volume:
            return _icon_volume;
        case icon_battery:
            return _icon_battery;
        case icon_melody:
            return _icon_melody;
        case icon_snake:
            return _icon_snake;
        case icon_return:
            return _icon_return;
    }
    return nullptr;
}

const uint8_t* get_bitmap(char c)
{
    if (c >= '0' && c <= '9')
    {
        return font_table[c - '0'];
    }
    if (c >= 'a' && c <= 'z')
    {
        c = c - 'a' + 'A';
    }
    if (c >= 'A' && c <= 'Z')
    {
        uint8_t position = letter_positions[c - 'A'];
        if (position < letter_count)
        {
            return letter_table[position].bitmap;
        }
    }
    return nullptr;
}
