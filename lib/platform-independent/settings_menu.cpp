#include <stdio.h>
#include "settings_menu.h"

void init_settings_menu(settings_menu_t *settings_menu)
{
    settings_menu->menu_position = BRIGHTNESS;
}

static void next_settings_menu_option(settings_menu_t *settings_menu)
{
    settings_menu->menu_position = (settings_t)(settings_menu->menu_position + 1);
    if(settings_menu->menu_position > SETTINGS_COUNT)
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
