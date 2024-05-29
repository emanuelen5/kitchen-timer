#include "state-machine.h"
#include "timer.h"
#include "util.h"

// These are provided by the program that includes the state machine
void UART_printf(const char *f, ...);
void set_counter(uint8_t v);
uint16_t millis(void);

void init_state_machine(state_machine_t *sm)
{
    sm->state = IDLE;
    reset_timer(&sm->timer);
    sm->millis_of_last_transition = millis();
}

static void set_state(state_machine_t *sm, state_t new_state)
{
    sm->millis_of_last_transition = millis();
    sm->state = new_state;
}

void service_state_machine(state_machine_t *sm)
{
    switch (sm->state)
    {
    case RINGING:
    {
        uint16_t time_in_state = millis() - sm->millis_of_last_transition;
        if (time_in_state > 2000)
        {
            reset_timer(&sm->timer);
            set_counter(0b000);
            set_state(sm, IDLE);
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
    switch (sm->state)
    {
    case IDLE:
        switch (event)
        {
        case PRESS:
            set_state(sm, RUNNING);
            break;
        case CW_ROTATION:
            change_original_timer(&sm->timer, 1);
            UART_printf("%d\n", sm->timer.original_time);
            break;
        case CCW_ROTATION:
            change_original_timer(&sm->timer, -1);
            UART_printf("%d\n", sm->timer.original_time);
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
        case PRESS:
            set_state(sm, PAUSED);
            break;
        case SECOND_TICK:
            increment_current_time(&sm->timer);
            UART_printf("%d\n", sm->timer.current_time);
            if (timer_is_finished(&sm->timer))
            {
                set_state(sm, RINGING);
                UART_printf("Alarm goes off!!!\n");
            }
            break;
        case LONG_PRESS:
            set_state(sm, IDLE);
            reset_timer(&sm->timer);
            break;
        default:
            break;
        }
        break;
    case PAUSED:
        break;
    case RINGING:
        switch (event)
        {
        case LONG_PRESS:
            /* code */
            break;

        default:
            break;
        }
        break;
    }

    if (sm->state == IDLE)
    {
        set_counter(sm->timer.original_time);
    }
    if (sm->state == RUNNING)
    {
        set_counter(sm->timer.current_time);
    }
}
