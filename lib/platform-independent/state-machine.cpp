#include "state-machine.h"
#include "timer.h"
#include "util.h"
#include "config.h"

// These are provided by the program that includes the state machine
void UART_printf(const char *f, ...);
void set_counter(uint8_t v);
uint16_t millis(void);

void set_state(state_machine_t *sm, state_t new_state)
{
    sm->millis_of_last_transition = millis();
    sm->prev_state = sm->state;
    sm->state = new_state;
}

static void reset_state_machine(state_machine_t *sm)
{
    reset_timer(&sm->timer);
    set_state(sm, IDLE);
}

void init_state_machine(state_machine_t *sm)
{
    sm->prev_state = IDLE;
    reset_state_machine(sm);
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
            reset_state_machine(sm);
            set_counter(0b000);
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

bool is_fast_event(event_t event)
{
    return event == CW_ROTATION_FAST || event == CCW_ROTATION_FAST;
}

typedef enum
{
    ccw,
    cw,
    none,
} rotation_dir_t;

rotation_dir_t event_to_rot_dir(event_t event)
{
    if (event == CW_ROTATION || event == CW_ROTATION_FAST)
    {
        return cw;
    }
    else if (event == CCW_ROTATION || event == CCW_ROTATION_FAST)
    {
        return ccw;
    }
    else
    {
        return none;
    }
}

static int16_t get_step_size(uint16_t original_time, rotation_dir_t dir, bool is_fast)
{
    const int16_t base_step = (original_time > 3600) ? 60 : 1;

    int16_t step_size = 0;
    switch (dir)
    {
        case cw:
            step_size = base_step;
            break;
        case ccw:
            step_size = -base_step;
            break;
        default:
            return 0;
    }

    const uint16_t fast_multiplier = 5;
    if (is_fast)
    {
        step_size *= fast_multiplier;
    }

    return step_size;
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
                copy_original_to_current_time(&sm->timer);
                set_state(sm, RUNNING);
                break;

            case CW_ROTATION:
            case CCW_ROTATION:
            case CW_ROTATION_FAST:
            case CCW_ROTATION_FAST: {
                const uint16_t step_size = get_step_size(sm->timer.original_time, event_to_rot_dir(event), is_fast_event(event));
                change_original_time( &sm->timer, step_size);
            }
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
                reset_state_machine(sm);
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
            reset_state_machine(sm);
            break;

        default:
            break;

        }
        break;
    case RINGING:
        switch (event)
        {
        case SINGLE_PRESS:
            reset_state_machine(sm);
            break;

        case LONG_PRESS:
            reset_state_machine(sm);
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
