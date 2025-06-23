#include "application.h"

 // These are provided by the program that includes the state machine 
 void UART_printf(const char *f, ...); 
 void set_counter(uint8_t v); 

static void pass_event_to_all_state_machines(application_t *app, event_t event);
static void make_new_state_machine(application_t *app);

void init_application(application_t *app)
{
    init_state_machine(&app->state_machines[0]);
    app->active_state_machine_index = 0;
    app->num_initialized_state_machines = 1;
}

void step_application(application_t *app, event_t event)
{
    if(event == DOUBLE_PRESS)
    {
        make_new_state_machine(app);
    }

    else if(event == SECOND_TICK)
    {
        pass_event_to_all_state_machines(app, event);
    }

    else
    {
        step_state(&app->state_machines[app->active_state_machine_index], event);
    }

}

static void make_new_state_machine(application_t *app)
{
    if(app->num_initialized_state_machines < MAX_TIMERS)
    {
        app->num_initialized_state_machines++;
        uint8_t new_state_machine_index = app->num_initialized_state_machines - 1;
        init_state_machine(&app->state_machines[new_state_machine_index]);   
        app->active_state_machine_index = new_state_machine_index;
        UART_printf("Num_Timers: %d\n", app->num_initialized_state_machines);
    }
    else
    {
        UART_printf("No more timers available.");
        //TODO: Blink a few times the whole Timer Indicator to show that no more timers can be created.
    }
}

static void pass_event_to_all_state_machines(application_t *app, event_t event)
{
    for (int8_t i = 0; i < MAX_TIMERS; i++)
    {
        step_state(&app->state_machines[i], event);
    }

}

void service_application(application_t *app)
{
    service_state_machine(&app->state_machines[app->active_state_machine_index]);
}
