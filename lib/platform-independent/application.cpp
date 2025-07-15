#include "application.h"

 // These are provided by the program that includes the state machine 
 void UART_printf(const char *f, ...); 
 void set_counter(uint8_t v); 

static void pass_event_to_all_state_machines(application_t *app, event_t event);
static void select_next_state_machine(application_t *app);
//static void debug_display(application_t *app);

void init_application(application_t *app)
{
    app->current_view = ACTIVE_TIMER_VIEW;
    for (int8_t i = 0; i < MAX_TIMERS; i++)
    {
        init_state_machine(app->state_machines[i]);
    }
    app->sm_count = 1;
    app->active_sm_index = 0;
}

void application_handle_event(application_t *app, event_t event)
{
    switch(event)
    {
        case CW_ROTATION:
            switch(app->active_sm->state)
            {
                case SET_TIME:
                    //TODO: up original_time
                    break;
                default:
                    //TODO: Change View()
                    break;
            }
            break;
            
        case CCW_ROTATION:
            switch(app->active_sm->state)
            {
                case SET_TIME:
                    //TODO: down original_time
                    break;
                default:
                    //TODO: Change View
                    break;
            }
            break;

        case SINGLE_PRESS:
            step_state(app->active_sm, event);
            break;

        case DOUBLE_PRESS:
            switch (app->active_sm->state)
            {
            case IDLE:
                //Do nothing
                break;
            case SET_TIME:
                //Do nothing
                break;
            default:
                //Make active the upper-most IDLE timer
                break;
            }
            break;
        
        case LONG_PRESS:
            switch(app->active_sm->state)
            {
                case IDLE:
                    //Do nothing
                    break;
                default:
                    step_state(app->active_sm, event);
                    break;
            }
            break;

        case CW_PRESSED_ROTATION:
            select_next_state_machine(app);
            break;

        case CCW_PRESSED_ROTATION:
            //TODO: Select_prev_state_machine();
            break;
        case SECOND_TICK:
            pass_event_to_all_state_machines(app, event);
            break;
        case TIMEOUT:
            //TODO: Make active the Timer that sent the TIMEOUT
        default:
            break;
    }
}

static void select_next_state_machine(application_t *app)
{
    app->active_sm_index++;
    if(app->active_sm_index >= MAX_TIMERS)
    {
        app->active_sm_index = 0;
    }
}

static void pass_event_to_all_state_machines(application_t *app, event_t event)
{
    for (int8_t i = 0; i < MAX_TIMERS; i++)
        step_state(app->state_machines[i], event);
}

/* static void debug_display(application_t *app)
{
    state_machine_t *active_sm = app->state_machines[app->active_sm_index];

    if (get_state(active_sm) == IDLE)
    {
        set_counter(get_original_time(active_sm));
    }
    if (get_state(active_sm) == RUNNING)
    {
        set_counter(get_current_time(active_sm));
    }
} */

void service_application(application_t *app)
{
    service_state_machine(app->state_machines[app->active_sm_index]);
}
