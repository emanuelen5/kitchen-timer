#include "application.h"

void handle_second_tick(application_t *app, event_t event);
void handle_sm_change(application_t *app, event_t event);

void init_application(application_t *app)
{
    app->active_state_machine_index = 0;
    for (int8_t i = 0; i < MAX_TIMERS; i++)
    {
        init_state_machine(&app->state_machines[i]);
    }
}

void step_application(application_t *app, event_t event)
{
    handle_second_tick(app, event);
    handle_sm_change(app, event);
}

void handle_second_tick(application_t *app, event_t event)
{
    if(event == SECOND_TICK)
    {
        for (int8_t i = 0; i < MAX_TIMERS; i++)
        {
            if(get_state(&app->state_machines[i]) == RUNNING)
            {
                increment_sm_current_timer(&app->state_machines[i]);
                if(get_state(&app->state_machines[i]) == RINGING)
                {
                    app->active_state_machine_index = i;
                }
            }
        }
    }
}

void handle_sm_change(application_t *app, event_t event)
{
    if(event == DOUBLE_PRESS)
    {
        app->active_state_machine_index++;
        if(app->active_state_machine_index >= MAX_TIMERS)
        {
            app->active_state_machine_index = 0;
        }
    }
}
