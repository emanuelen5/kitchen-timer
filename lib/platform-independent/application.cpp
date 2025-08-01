#include "application.h"

 // These are provided by the program that includes the state machine 
 void UART_printf(const char *f, ...); 
 void set_counter(uint8_t v); 

static void pass_event_to_all_state_machines(application_t *app, event_t event);
static void select_previous_state_machine(application_t *app);
static void select_next_state_machine(application_t *app);
static void change_to_previous_view(application_t *app);
static void change_to_next_view(application_t *app);
static void try_to_open_new_timer(application_t* app);
static void print_state(state_t state);
static void print_event(event_t event);

void init_application(application_t *app)
{
    app->current_view = ACTIVE_TIMER_VIEW;
    for (int8_t i = 0; i < MAX_TIMERS; i++)
    {
        init_state_machine(&app->state_machines[i]);
    }
    app->current_active_sm = 0;
}

void application_handle_event(application_t *app, event_t event)
{
    state_machine_t* active_sm = &app->state_machines[app->current_active_sm];

    if(event == CW_ROTATION && active_sm->state != SET_TIME)
    {
        change_to_next_view(app);
    } else if (event == CCW_ROTATION && active_sm->state != SET_TIME)
    {
        change_to_previous_view(app);
    } else if (event == DOUBLE_PRESS)
    {
        try_to_open_new_timer(app);
    } else if (event == CW_PRESSED_ROTATION)
    {
        select_next_state_machine(app);

    } else if (event == CCW_PRESSED_ROTATION)
    {
        select_previous_state_machine(app);

    } else if (event == SECOND_TICK) 
    {
        pass_event_to_all_state_machines(app, event);
    }
    else
    {
        state_machine_handle_event(active_sm, event);
    }
}

void service_application(application_t *app)
{
    bool existing_ringing_timer = false;
    for (uint8_t i = 0; i < MAX_TIMERS; i++)
    {
        bool is_ringing = app->state_machines[i].state == RINGING;
        if(is_ringing && !existing_ringing_timer)
        {
            app->current_active_sm = i;
            existing_ringing_timer = true;
        }
        service_state_machine(&app->state_machines[i]);
    }
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

static void change_to_previous_view(application_t *app)
{
    const uint8_t first_view = 0;
    if(app->current_view > first_view)
    {
        app->current_view = (application_view_t)(app->current_view - 1);
    }
}

static void change_to_next_view(application_t *app)
{
    const uint8_t last_view = VIEW_COUNT - 1;
    if(app->current_view < last_view)
    {
        app->current_view = (application_view_t)(app->current_view + 1);
    }
}

static void try_to_open_new_timer(application_t* app)
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

[[maybe_unused]] static void print_state(state_t state)
{
    switch (state)
    {
        case IDLE:
            UART_printf("IDLE");
            break;
        case SET_TIME:
            UART_printf("SET_TIME");
            break;
        case RUNNING:
            UART_printf("RUNNING");
            break;
        case PAUSED:
            UART_printf("PAUSED");
            break;
        case RINGING:
            UART_printf("RINGING");
            break;
        default:
            UART_printf("UNKNOWN STATE");
            break;
    }
}

[[maybe_unused]] static void print_event(event_t event)
{
    switch(event)
    {
        case SINGLE_PRESS:
            UART_printf("SINGLE_PRESS");
            break;
        case CW_ROTATION:
            UART_printf("CW_ROTATION");
            break;
        case CCW_ROTATION:
            UART_printf("CCW_ROTATION");
            break;
        case DOUBLE_PRESS:
            UART_printf("DOUBLE_PRESS");
            break;
        case LONG_PRESS:
            UART_printf("LONG_PRESS");
            break;
        case CW_PRESSED_ROTATION:
            UART_printf("CW_PRESSED_ROTATION");
            break;
        case CCW_PRESSED_ROTATION:
            UART_printf("CCW_PRESSED_ROTATION");
            break;
        case SECOND_TICK:
            UART_printf("SECOND_TICK");
            break;
    }
}
