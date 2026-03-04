#ifndef FAT_FONT_H
#define FAT_FONT_H

#define FATFONT_WIDTH 6
#define FATFONT_HEIGHT 7

const uint8_t* get_bitmap(char c);

enum icons
{
    icon_brightness,
    icon_battery,
    icon_volume,
    icon_melody,
    icon_snake,
    icon_return
};

const uint8_t* get_icon_bitmap(enum icons);

#endif // FAT_FONT_H
