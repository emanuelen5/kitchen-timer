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
    settings_t selected_setting;
} settings_menu_t;

typedef void (*change_settings_views_cb_t)(void *app_argument, settings_t selected_setting);
typedef void (*change_back_to_settings_menu_view_cb_t)(void *app_argument, settings_menu_t *settings_menu);

void init_settings_menu(settings_menu_t *settings_menu);
void settings_menu_event_handling(settings_menu_t *settings_menu, change_settings_views_cb_t change_to_a_setting_view_cb, void *app_argument, event_t event);
void brightness_setting_event_handling(settings_menu_t *settings_menu, change_back_to_settings_menu_view_cb_t change_back_to_settings_menu_view_cb, void *app_argument, event_t event);
void volume_setting_event_handling(settings_menu_t *settings_menu, change_back_to_settings_menu_view_cb_t change_back_to_settings_menu_view_cb, void *app_argument, event_t event);

#endif //SETTINGS_MENU_H
