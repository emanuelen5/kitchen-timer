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

const uint8_t encoder_rotation_interval_buffer_size = 2; 
static uint16_t interval_buffer[encoder_rotation_interval_buffer_size];
static uint8_t interval_index = 0;
static uint32_t last_encoder_time = 0;
const uint8_t fast_encoder_step_threshold = 30;
const uint8_t fast_step = 5;
static void handle_encoder_rotation(state_machine_t *sm, event_t event)
{
    uint32_t now = millis();
    uint32_t interval = now - last_encoder_time;
    last_encoder_time = now;

    interval_buffer[interval_index] = interval;
    interval_index = (interval_index + 1 ) % encoder_rotation_interval_buffer_size;

    uint32_t sum = 0;
    for (uint8_t i = 0; i < encoder_rotation_interval_buffer_size; ++i)
    {
        sum += interval_buffer[i];
    }
    uint32_t average_interval = sum / encoder_rotation_interval_buffer_size;

    int step_size = (average_interval < fast_encoder_step_threshold) ? fast_step : 1;

    if (event == CCW_ROTATION) step_size *= (-1);

    change_original_time(&sm->timer, step_size);
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
                handle_encoder_rotation(sm, event);
                break;

            case CCW_ROTATION:
                handle_encoder_rotation(sm, event);
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
