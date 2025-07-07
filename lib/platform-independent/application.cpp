#include "application.h"

// These are provided by the program that includes the state machine
void UART_printf(const char *f, ...);
void set_counter(uint8_t v);

static void pass_event_to_all_state_machines(application_t *app, event_t event);
static void select_next_state_machine(application_t *app);
static void debug_display(application_t *app);

void init_application(application_t *app)
{
    app->active_state_machine_index = 0;

    for (int8_t i = 0; i < MAX_TIMERS; i++)
    {
        app->state_machines[i].reset();
    }
}

void step_application(application_t *app, event_t event)
{
    if (event == DOUBLE_PRESS)
    {
        select_next_state_machine(app);
    }

    else if (event == SECOND_TICK)
    {
        pass_event_to_all_state_machines(app, event);
    }

    else
    {
        app->state_machines[app->active_state_machine_index].handle_event(event);
    }

    debug_display(app);
}

static void select_next_state_machine(application_t *app)
{
    app->active_state_machine_index++;
    if (app->active_state_machine_index >= MAX_TIMERS)
    {
        app->active_state_machine_index = 0;
    }
}

static void pass_event_to_all_state_machines(application_t *app, event_t event)
{
    for (int8_t i = 0; i < MAX_TIMERS; i++)
        app->state_machines[i].handle_event(event);
}

static void debug_display(application_t *app)
{
    KitchenTimerStateMachine &active_sm = app->state_machines[app->active_state_machine_index];

    if (active_sm.get_state() == IDLE)
    {
        set_counter(active_sm.get_target_time());
    }
    if (active_sm.get_state() == RUNNING)
    {
        set_counter(active_sm.get_current_time());
    }
}

void service_application(application_t *app)
{
    app->state_machines[app->active_state_machine_index].service();
}
