#include "interaction.h"
#include "application.h"

extern application_t app;

void press(void)
{
    application_handle_event(&app, SINGLE_PRESS);
}

void double_press(void)
{
    application_handle_event(&app, DOUBLE_PRESS);
}

void long_press(void)
{
    application_handle_event(&app, LONG_PRESS);
}

void rotate(rotation_dir_t dir, bool is_fast)
{
    event_t event;
    if (dir == cw)
    {
        event = is_fast ? CW_ROTATION_FAST : CW_ROTATION;
    }
    else
    {
        event = is_fast ? CCW_ROTATION_FAST : CCW_ROTATION;
    }

    application_handle_event(&app, event);
}

void rotate_pressed(rotation_dir_t dir)
{
    event_t event;
    if (dir == cw)
    {
        event = CW_PRESSED_ROTATION;
    }
    else
    {
        event = CCW_PRESSED_ROTATION;
    }

    application_handle_event(&app, event);
}
