#include "application.h"
#include "led-counter.h"
#include "UART.h"

static void pass_event_to_all_state_machines(application_t *app, event_t event);
static void select_next_state_machine(application_t *app);
static void debug_display(application_t *app);

void init_application(application_t *app)
{
    app->active_state_machine_index = 0;
    for (int8_t i = 0; i < MAX_TIMERS; i++)
    {
        init_state_machine(&app->state_machines[i]);
    }
}

void step_application(application_t *app, event_t event)

static void select_next_state_machine(application_t *app)
{
    app->active_state_machine_index++;
}

static void pass_event_to_all_state_machines(application_t *app, event_t event)
{
    for (int8_t i = 0; i < MAX_TIMERS; i++)
        step_state(&app->state_machines[i], event);
}

static void debug_display(application_t *app)
{
    state_machine_t *active_sm = &app->state_machines[app->active_state_machine_index];

    if (get_state(active_sm) == IDLE)
    {
        set_counter(get_original_time(active_sm));
    }
    if (get_state(active_sm) == RUNNING)
    {
        set_counter(get_current_time(active_sm));
    }
}

void service_application(application_t *app)
{
    service_state_machine(&app->state_machines[app->active_state_machine_index]);
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
                    UART_printf("T%d: Alarm goes off!!!\n", app->active_state_machine_index);
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
