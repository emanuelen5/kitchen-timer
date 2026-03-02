#include <stdio.h>
#include "settings_menu.h"
#include "UART.h"
#include "settings.h"

void save_byte_setting(uint8_t setting, eeprom_address address);
void max72xx_set_intensity(uint8_t intensity_level);

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
