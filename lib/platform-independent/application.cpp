#include "application.h"

 // These are provided by the program that includes the state machine 
 void UART_printf(const char *f, ...); 
 void set_counter(uint8_t v); 

static void pass_event_to_all_state_machines(application_t *app, event_t event);
static void select_state_machine(application_t *app, event_t event);
static void change_view(application_t *app, event_t event);
static void open_new_timer(application_t* app);
//static void debug_display(application_t *app);

void init_application(application_t *app)
{
    app->current_view = ACTIVE_TIMER_VIEW;
    for (int8_t i = 0; i < MAX_TIMERS; i++)
    {
        init_state_machine(&app->state_machines[i]);
    }
    app->current_active_sm = 0;
    app->active_sm = &app->state_machines[app->current_active_sm];
}

void application_handle_event(application_t *app, event_t event)
{
    switch(event)
    {
        case CW_ROTATION:
            switch(app->active_sm->state)
            {
                case SET_TIME:
                    state_machine_handle_event(app->active_sm, event);
                    break;
                default:
                    change_view(app, event);
                    break;
            }
            break;
            
        case CCW_ROTATION:
            switch(app->active_sm->state)
            {
                case SET_TIME:
                    state_machine_handle_event(app->active_sm, event);
                    break;
                default:
                    change_view(app, event);
                    break;
            }
            break;

        case SINGLE_PRESS:
            state_machine_handle_event(app->active_sm, event);
            break;

        case DOUBLE_PRESS:
            open_new_timer(app);
            break;
        
        case LONG_PRESS:
            switch(app->active_sm->state)
            {
                case IDLE:
                    //Do nothing
                    break;
                default:
                    state_machine_handle_event(app->active_sm, event);
                    break;
            }
            break;

        case CW_PRESSED_ROTATION:
            select_state_machine(app, event);
            break;

        case CCW_PRESSED_ROTATION:
            select_state_machine(app, event);
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

void service_application(application_t *app)
{
    service_state_machine(&app->state_machines[app->current_active_sm]);
}

static void select_state_machine(application_t *app, event_t event)
{
    const uint8_t first_sm = 0;
    const uint8_t last_sm = MAX_TIMERS - 1;

    switch(event)
    {
        case CW_PRESSED_ROTATION:
            if(app->current_active_sm < last_sm)
            {
                app->current_active_sm++;
            }
            break;

        case CCW_PRESSED_ROTATION:
            if(app->current_active_sm > first_sm)
            {
                app->current_active_sm--;
            }
            break;

        default:
            break;
    }
}

static void pass_event_to_all_state_machines(application_t *app, event_t event)
{
    for (int8_t i = 0; i < MAX_TIMERS; i++)
        state_machine_handle_event(&app->state_machines[i], event);
}

/* static void debug_display(application_t *app)
{
    state_machine_t *active_sm = app->state_machines[app->current_active_sm];

    if (get_state(active_sm) == IDLE)
    {
        set_counter(get_original_time(active_sm));
    }
    if (get_state(active_sm) == RUNNING)
    {
        set_counter(get_current_time(active_sm));
    }
} */


static void change_view(application_t *app, event_t event)
{
    const uint8_t first_view = 0;
    const uint8_t last_view = APPLICATION_VIEWS_COUNT - 1;

    switch (event)
    {
        case CW_ROTATION:
            if(app->current_view < last_view)
            {
                app->current_view = (application_view)(app->current_view + 1);
            }
            break;

        case CCW_ROTATION:
            if(app->current_view > first_view)
            {
                app->current_view = (application_view)(app->current_view - 1);
            }
            break;
        
        default:
            break;
    }
}

static void open_new_timer(application_t* app)
{
    bool new_timer_found = false;
    for(int i = 0; i < MAX_TIMERS; i++)
    {
        if(app->state_machines[i].state == IDLE)
        {
            app->current_active_sm = i;
            new_timer_found = true;
            return;
        }
    }
    if (new_timer_found)
    {
        //blink all timers indicators three times.
    }
}
