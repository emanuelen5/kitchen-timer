#include "settings_menu.h"
#include <stdio.h>
#include "settings.h"

void save_byte_setting(uint8_t setting, eeprom_address address);
void max72xx_set_intensity(uint8_t intensity_level);

void init_settings_menu(settings_menu_t *settings_menu)
{
    settings_menu->current_menu_position = BRIGHTNESS;
}

static void next_settings_menu_option(settings_menu_t *settings_menu)
{
    if (settings_menu->current_menu_position < (settings_t)(SETTINGS_COUNT - 1))
    {
        settings_menu->current_menu_position = (settings_t)(settings_menu->current_menu_position + 1);
    }
}

static void previous_setting_menu_option(settings_menu_t *settings_menu)
{
    if (settings_menu->current_menu_position > BRIGHTNESS)
    {
        settings_menu->current_menu_position = (settings_t)(settings_menu->current_menu_position - 1);
    }
}

void settings_menu_event_handling(settings_menu_t *settings_menu, change_settings_views_cb_t change_to_a_setting_view_cb, void *app_argument, event_t event)
{
    settings_t position = settings_menu->current_menu_position;
    settings_t selected_setting = settings_menu->selected_setting;

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
            selected_setting = position;
            change_to_a_setting_view_cb(app_argument, selected_setting);
            break;
        
        default:
            break;
    }
}

void volume_setting_event_handling(settings_menu_t *settings_menu, change_back_to_settings_menu_view_cb_t change_back_to_settings_menu_view_cb, void *app_argument, event_t event)
{
    switch (event)
    {
        case CW_ROTATION:
        case CW_ROTATION_FAST:
            //Increase volume
            break;

        case CCW_ROTATION:
        case CCW_ROTATION_FAST:
            //Decrease volume
            break;

        case SINGLE_PRESS:
            //Apply changes
            change_back_to_settings_menu_view_cb(app_argument, settings_menu);
            break;
        
        default:
            break;
    }
}
