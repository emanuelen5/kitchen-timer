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

typedef enum
{
    SETTINGS_BRIGHTNESS_VIEW,
} settings_views_t;

typedef struct
{
    settings_t menu_position;
    settings_t settings_sellection;
} settings_menu_t;

typedef void (*exit_settings_menu_cb_t)(void *app_argument);

void init_settings_menu(settings_menu_t *settings_menu, exit_settings_menu_cb_t exit_settings_menu_cb, void *app_argument);
void settings_menu_event_handling(settings_menu_t *settings_menu, event_t event);

#endif //SETTINGS_MENU_H
