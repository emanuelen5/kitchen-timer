#include <stdio.h>
#include "settings_menu.h"

void init_settings_menu(settings_menu_t *settings_menu)
{
    settings_menu->menu_position = BRIGHTNESS;
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

void settings_menu_event_handling(settings_menu_t *settings_menu, const change_settings_views_callbacks_t *change_settings_views_callbacks, void *app_argument, event_t event)
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
                    //TODO
                    break;
                case BACK:
                    change_settings_views_callbacks->exit_settings_menu(app_argument);
                    break;
                default:
                    break;
                }

                settings_menu->menu_position = settings_menu->settings_sellection;
            break;
        
        default:
            break;
    }
}
