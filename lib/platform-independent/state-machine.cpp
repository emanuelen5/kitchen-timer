#include "state-machine.h"
#include "timer.h"
#include "util.h"
#include "config.h"

// These are provided by the program that includes the state machine
void UART_printf(const char *f, ...);
void set_counter(uint8_t v);
uint16_t millis(void);

void init_state_machine(state_machine_t *sm)
{
    sm->prev_state = IDLE;
    set_state(sm, IDLE);
    reset_timer(&sm->timer);
}

void set_state(state_machine_t *sm, state_t new_state)
{
    sm->millis_of_last_transition = millis();
    sm->prev_state = sm->state;
    sm->state = new_state;
}

void service_state_machine(state_machine_t *sm)
{
    switch (sm->state)
    {
    case RINGING:
    {
        uint16_t time_in_ringing_state = millis() - sm->millis_of_last_transition;
        if (time_in_ringing_state >= RINGING_TIMEOUT)
        {
            reset_timer(&sm->timer);
            set_counter(0b000);
            set_state(sm, IDLE);
        }
        else
        {
            bool is_in_odd_128ms_period = time_in_ringing_state & bit(7);
            if (is_in_odd_128ms_period)
            {
                set_counter(0b111);
            }
            else // is in even 128 ms period
            {
                set_counter(0b000);
            }
        }
    }
    break;
    default:
        break;
    }
}

void state_machine_handle_event(state_machine_t *sm, event_t event)
{
    switch (sm->state)
    {
    case IDLE:
        switch (event)
        {
            case SINGLE_PRESS:
                set_state(sm, SET_TIME);
                break;
            case LONG_PRESS:
                //Does nothing
                break;
            default:
                break;
        }
        break;
    
    case SET_TIME:
        switch (event)
        {
            case SINGLE_PRESS:
                set_current_time(&sm->timer);
                set_state(sm, RUNNING);
                break;
            case CW_ROTATION:
                change_original_time(&sm->timer, 1);
                break;
            case CCW_ROTATION:
                change_original_time(&sm->timer, -1);
                break;
            case LONG_PRESS:
                reset_timer(&sm->timer);
                break;
            default:
                break;
        }
        break;
        
    case RUNNING:
        switch (event)
        {
        case SINGLE_PRESS:
            set_state(sm, PAUSED);
            break;
        case LONG_PRESS:
            reset_timer(&sm->timer);
            set_state(sm, IDLE);
            break;
        case SECOND_TICK:
            decrement_current_time(&sm->timer);
            if (timer_is_finished(&sm->timer))
            {
                set_state(sm, RINGING);
            }
            break;
        default:
            break;
        }
        break;
    case PAUSED:
        switch (event)
        {
        case SINGLE_PRESS:
            set_state(sm, RUNNING);
            break;
        case LONG_PRESS:
            reset_timer(&sm->timer);
            set_state(sm, IDLE);
            break;
        default:
            break;
        }
        break;
    case RINGING:
        switch (event)
        {
        case SINGLE_PRESS:
            reset_timer(&sm->timer);
            set_state(sm, IDLE);
            break;
        case LONG_PRESS:
            reset_timer(&sm->timer);
            set_state(sm, IDLE);
            break;
        default:
            break;
        }
        break;
    }
}

uint16_t get_original_time(state_machine_t *sm)
{
    return sm->timer.original_time;
}

uint16_t get_current_time(state_machine_t *sm)
{
    return timer_get_current_time(&sm->timer);
}

state_t get_state(state_machine_t *sm)
{
    return sm->state;
}
