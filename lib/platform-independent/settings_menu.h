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
    settings_t menu_position;
    settings_t settings_sellection;
} settings_menu_t;

typedef struct
{
    void (*exit_settings_menu)(void *app_argument);
} change_settings_views_callbacks_t;

void init_settings_menu(settings_menu_t *settings_menu);
void settings_menu_event_handling(settings_menu_t *settings_menu, const change_settings_views_callbacks_t *change_settings_views_callbacks, void *app_argument, event_t event);

#endif //SETTINGS_MENU_H
