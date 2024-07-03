#include "state-machine.h"
#include "timer.h"
#include "util.h"

// These are provided by the program that includes the state machine
void UART_printf(const char *f, ...);
void set_counter(uint8_t v);
uint16_t millis(void);

void init_state_machine(state_machine_t *sm)
{
    for (uint8_t i = 0; i < MAX_TIMERS; i++)
    {
        set_state(&sm->timers[i], IDLE);
        reset_timer(&sm->timers[i]);
    }
    sm->current_timer_index = 0;
    sm->last_ringing_timer_index = 0;
}


void service_state_machine(state_machine_t *sm)
{
    state_machine::timer_t *current_timer = &sm->timers[sm->current_timer_index];

    if(sm->timers[sm->last_ringing_timer_index].state == RINGING)
    {
        sm->current_timer_index = sm->last_ringing_timer_index;
    }

    switch (current_timer->state)
    {
    case RINGING:
    {
        UART_printf("T%d: Alarm goes off!!!\n", sm->current_timer_index);
        uint16_t time_in_state = millis() - current_timer->millis_of_last_transition;
        if (time_in_state >= 2000)
        {
            reset_timer(current_timer);
            //TODO: At these point the clock needs to go to the next available timer.
            set_counter(0b000);
        }
        else
        {
            bool is_in_odd_128ms_period = time_in_state & bit(7);
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

void step_state(state_machine_t *sm, event_t event)
{
    state_machine::timer_t *current_timer = &sm->timers[sm->current_timer_index];

    switch (current_timer->state)
    {
    case IDLE:
        switch (event)
        {
        case SINGLE_PRESS:
            set_state(current_timer, RUNNING);
            break;
        case CW_ROTATION:
            change_original_time(current_timer, 1);
            UART_printf("T%d: %d\n", sm->current_timer_index, current_timer->original_time);
            break;
        case CCW_ROTATION:
            change_original_time(current_timer, -1);
            UART_printf("T%d: %d\n", sm->current_timer_index, current_timer->original_time);
            break;
        case LONG_PRESS:
            reset_timer(current_timer);
            break;
        default:
            break;
        }
        break;
    case RUNNING:
        switch (event)
        {
        case SINGLE_PRESS:
            set_state(current_timer, PAUSED);
            UART_printf("Pause\n");
            break;
        case LONG_PRESS:
            reset_timer(current_timer);
            set_state(current_timer, IDLE);
            break;
        case SECOND_TICK:
            for(uint8_t i = 0; i < MAX_TIMERS; i++)
            {
                state_machine::timer_t *timer = &sm->timers[i];
                if(timer->state == RUNNING)
                {
                    increment_current_time(timer);
                    if (timer_is_finished(timer))
                    {
                        set_state(timer, RINGING);
                        sm->last_ringing_timer_index = i;
                    }       
                }
            }
            UART_printf("T%d: %d\n", sm->current_timer_index, current_timer->current_time);
            break;
        default:
            break;
        }
        break;
    case PAUSED:
        switch (event)
        {
        case SINGLE_PRESS:
            set_state(current_timer, RUNNING);
            break;
        case LONG_PRESS:
            reset_timer(current_timer);
            set_state(current_timer, IDLE);
            break;
        default:
            break;
        }
        break;
    case RINGING:
        switch (event)
        {
        case SINGLE_PRESS:
            reset_timer(current_timer);
            break;
        default:
            break;
        }
        break;
    }

    if (current_timer->state == IDLE)
    {
        set_counter(current_timer->original_time);
    }
    if (current_timer->state == RUNNING)
    {
        set_counter(current_timer->current_time);
    }
}
