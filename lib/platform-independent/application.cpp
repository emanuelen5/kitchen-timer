#include "application.h"
#include "melody.h"
#include "settings.h"
#include "settings_menu.h"

void init_application(application_t *app)
{
    app->current_view = ACTIVE_TIMER_VIEW;

    for (int8_t i = 0; i < MAX_TIMERS; i++)
    {
        init_state_machine(&app->state_machines[i]);
        app->previous_sm_states[i] = app->state_machines[i].state;
    }

    uint8_t last_brightness_setting;
    load_byte_setting(&last_brightness_setting, EEPROM_BRIGHTNESS_ADDR);
    app->brightness = last_brightness_setting;

    uint8_t last_volume_setting;
    load_byte_setting(&last_volume_setting, EEPROM_VOLUME_ADDR);
    app->buzzer.set_volume(last_volume_setting);

    app->power_save.init(&app->brightness);

    app->current_active_sm = 0;
    set_state(&app->state_machines[0], SET_TIME);

    init_settings_menu(&app->settings_menu);
}

static bool sm_transitioned_into_state(application_t *app, uint8_t sm_index, state_t into)
{
    state_t current_state = app->state_machines[sm_index].state;
    state_t previous_state = app->previous_sm_states[sm_index];
    return current_state == into && previous_state != into;
}

static bool sm_transitioned_from_state(application_t *app, uint8_t sm_index, state_t from)
{
    state_t current_state = app->state_machines[sm_index].state;
    state_t previous_state = app->previous_sm_states[sm_index];
    return current_state != from && previous_state == from;
}

void service_application(application_t *app)
{
    for (uint8_t i = 0; i < MAX_TIMERS; i++)
        service_state_machine(&app->state_machines[i]);

    app->buzzer.service();

    for (uint8_t i = 0; i < MAX_TIMERS; i++)
    {
        if (sm_transitioned_into_state(app, i, RINGING))
        {
            app->current_active_sm = i;
            app->buzzer.start_melody(beeps, 10);
            app->power_save.handle_event(activity);
            break;
        }
        else if (sm_transitioned_from_state(app, i, RINGING))
        {
            app->buzzer.stop();
        }
    }

    for (uint8_t i = 0; i < MAX_TIMERS; i++)
        app->previous_sm_states[i] = app->state_machines[i].state;
}

static void select_previous_state_machine(application_t *app)
{
    const uint8_t first_sm = 0;
    for (int8_t i = app->current_active_sm - 1; i >= first_sm; i--)
    {
        if (app->state_machines[i].state != IDLE)
        {
            app->current_active_sm = i;
            break;
        }
    }
}

static void select_next_state_machine(application_t *app)
{
    const uint8_t last_sm = MAX_TIMERS - 1;
    for (uint8_t i = app->current_active_sm + 1; i <= last_sm; i++)
    {
        if (app->state_machines[i].state != IDLE)
        {
            app->current_active_sm = i;
            break;
        }
    }
}

static void pass_event_to_all_state_machines(application_t *app, event_t event)
{
    for (int8_t i = 0; i < MAX_TIMERS; i++)
        state_machine_handle_event(&app->state_machines[i], event);
}

// static void change_to_previous_view(application_t *app)
// {
//     const uint8_t first_view = 0;
//     if (app->current_view > first_view)
//     {
//         app->current_view = (application_view_t)(app->current_view - 1);
//     }
// }
// 
// static void change_to_next_view(application_t *app)
// {
//     const uint8_t last_view = VIEW_COUNT - 1;
//     if (app->current_view < last_view)
//     {
//         app->current_view = (application_view_t)(app->current_view + 1);
//     }
// }

static void try_to_open_new_timer(application_t *app)
{
    for (int i = 0; i < MAX_TIMERS; i++)
    {
        if (app->state_machines[i].state == IDLE)
        {
            app->current_active_sm = i;
            set_state(&app->state_machines[i], SET_TIME);
            return;
        }
    }
}

static bool any_timer_has_state(application_t *app, state_t state)
{
    for (int i = 0; i < MAX_TIMERS; i++)
    {
        if (app->state_machines[i].state == state)
            return true;
    }
    return false;
}

static void exit_settings_menu_cb(void *app_argument)
{
    application_t *app = (application_t *)app_argument;
    app->current_view = ACTIVE_TIMER_VIEW;
}

const change_settings_views_callbacks_t change_settings_views_callbacks
{
    .exit_settings_menu = exit_settings_menu_cb
};

void application_handle_event(application_t *app, event_t event)
{
    state_machine_t *active_sm = &app->state_machines[app->current_active_sm];
    uint16_t *original_time = &active_sm->timer.original_time;

    if (is_interactive_event(event))
    {
        app->power_save.handle_event(PowerSaveEvent::activity);
    }

    if (event == SECOND_TICK)
    {
        pass_event_to_all_state_machines(app, event);
        if (any_timer_has_state(app, RINGING))
            app->power_save.handle_event(PowerSaveEvent::activity);
        else if (!any_timer_has_state(app, RUNNING))
            app->power_save.handle_event(PowerSaveEvent::nothing_running_last_second);
        else
            app->power_save.handle_event(PowerSaveEvent::no_activity_last_second);
    }
    else
    {
        switch (app->current_view)
        {
            case ACTIVE_TIMER_VIEW:
                if (event == DOUBLE_PRESS && *original_time != 0 && active_sm->state != RINGING)
                {
                    try_to_open_new_timer(app);
                }
                else if (event == DOUBLE_PRESS && active_sm->state == SET_TIME && *original_time == 0)
                {
                    app->current_view = SETTINGS_MENU_VIEW;
                }
                else if (event == CW_PRESSED_ROTATION && active_sm->state != IDLE)
                {
                    select_next_state_machine(app);
                }
                else if (event == CCW_PRESSED_ROTATION && active_sm->state != IDLE)
                {
                    select_previous_state_machine(app);
                }
                else
                {
                    state_machine_handle_event(active_sm, event);
                }
                break;

            case SETTINGS_MENU_VIEW:
                settings_menu_event_handling(&app->settings_menu, &change_settings_views_callbacks, app, event);
                break;

            case BRIGHTNESS_SETTING_VIEW:
                break;
            
            default:
                //Do nothing
                break;
        }
    }
}
