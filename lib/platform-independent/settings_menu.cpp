#include <stdio.h>
#include "settings_menu.h"

static exit_settings_menu_cb_t exit_settings_menu;
static void *app;

void init_settings_menu(settings_menu_t *settings_menu, exit_settings_menu_cb_t settings_menu_exit_cb, void *app_argument)
{
    settings_menu->menu_position = BRIGHTNESS;

    exit_settings_menu = settings_menu_exit_cb;
    app = app_argument;
}

static void next_settings_menu_option(settings_menu_t *settings_menu)
{
    settings_menu->menu_position = (settings_t)(settings_menu->menu_position + 1);
    if(settings_menu->menu_position > SETTINGS_COUNT - 1)
    {
        settings_menu->menu_position = BRIGHTNESS;
    }   
}

static void previous_setting_menu_option(settings_menu_t *settings_menu)
{
    settings_menu->menu_position = (settings_t)(settings_menu->menu_position - 1);
    if(settings_menu->menu_position < 0)
    {
        settings_menu->menu_position = (settings_t)(SETTINGS_COUNT - 1);
    }
}

static void sellect_settings_option(settings_menu_t *settings_menu)
{
    switch(settings_menu->menu_position)
    {
    case BRIGHTNESS: 
        break;
    case VOLUME: 
        break;
    case BATTERY_V: 
        break;
    case MELODY: 
        break;
    case SNAKE: 
        break;
    case BACK:
        exit_settings_menu(app);
        break;
    default:
        break;
    }

    settings_menu->menu_position = settings_menu->settings_sellection;
}


void settings_menu_event_handling(settings_menu_t *settings_menu, event_t event)
{
    switch (event)
    {
        case CW_ROTATION:
        case CW_ROTATION_FAST:
            next_settings_menu_option(settings_menu);
            break;

        case CCW_ROTATION:
        case CCW_ROTATION_FAST:
            previous_setting_menu_option(settings_menu);
            break;

        case SINGLE_PRESS:
            sellect_settings_option(settings_menu);
            break;
        
        default:
            break;
    }
}
