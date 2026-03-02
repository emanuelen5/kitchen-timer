#include <stdint.h>
#include "fat_font.h"
#include <avr/pgmspace.h>

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

const uint8_t _icon_brightness[] PROGMEM = {
    0x00, 0x00,
    0x01, 0x00,
    0x21, 0x08,
    0x10, 0x10,
    0x03, 0x80,
    0x07, 0xc0,
    0x0f, 0xe0,
    0x6f, 0xee,
    0x0f, 0xe0,
    0x07, 0xc0,
    0x03, 0x80,
    0x10, 0x10,
    0x21, 0x08,
    0x01, 0x00,
    0x00, 0x00,
    0x00, 0x00
};

const uint8_t _icon_volume[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x03, 0x30, 0x07, 0x18, 0x3f, 0x4c, 0x3f, 0x64, 0x3f, 0x24, 0x3f, 0x64, 0x3f, 0x4c, 0x07, 0x18, 0x03, 0x30, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t _icon_battery[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x60, 0x00, 0xc0, 0x01, 0x80, 0x03, 0x00, 0x07, 0xf0, 0x0f, 0xe0, 0x1f, 0xc0, 0x01, 0x80, 0x03, 0x00, 0x06, 0x00, 0x0c, 0x00, 0x08, 0x00, 0x00, 0x00
};

const uint8_t _icon_melody[] PROGMEM = {
    0x00, 0x00, 0x00, 0x18, 0x00, 0xf8, 0x07, 0xf8, 0x07, 0x88, 0x04, 0x08, 0x04, 0x08, 0x04, 0x08, 0x04, 0x08, 0x04, 0x08, 0x1c, 0x38, 0x3c, 0x78, 0x3c, 0x78, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00
};

const uint8_t _icon_snake[] PROGMEM = {
    0x00, 0x00, 0x01, 0x80, 0x03, 0xc0, 0x07, 0xe0, 0x06, 0x60, 0x06, 0x70, 0x36, 0x76, 0x7e, 0x6f, 0x7c, 0xec, 0x30, 0xcc, 0x01, 0xcc, 0x01, 0xcc, 0x00, 0xfc, 0x00, 0x78, 0x00, 0x30, 0x00, 0x00
};

const uint8_t _icon_return[] PROGMEM= {
    0x00, 0x00, 0x00, 0x20, 0x00, 0x30, 0x07, 0xf8, 0x0f, 0xfc, 0x1f, 0xfe, 0x3f, 0xfc, 0x3f, 0xf8, 0x3c, 0x30, 0x1c, 0x20, 0x1c, 0x00, 0x0e, 0x00, 0x06, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00
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
    switch (c)
    {
        case 'h':
            return char_h;
    }
    return nullptr;
}
