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

void settings_menu_event_handling(settings_menu_t *settings_menu, change_settings_views_cb_t change_to_a_setting_view_cb, void *app_argument, event_t event)
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
            settings_menu->selected_setting = settings_menu->menu_position;
            change_to_a_setting_view_cb(app_argument, settings_menu->selected_setting);
            break;
        
        default:
            break;
    }
}
            break;
        
        default:
            break;
    }
}
