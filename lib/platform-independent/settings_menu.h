#ifndef SETTINGS_MENU_H
#define SETTINGS_MENU_H

#include "events.h"

typedef enum
{
    BRIGHTNESS,
    VOLUME,
    BATTERY_V,
    MELODY,
    SNAKE,
    BACK,
    SETTINGS_COUNT
} settings_t;

typedef struct
{
    settings_t current_menu_position;
    settings_t selected_setting;
} settings_menu_t;

void init_settings_menu(settings_menu_t *settings_menu);

#endif //SETTINGS_MENU_H
