#include "state-machine.h"
#include "timer.h"
#include "util.h"

static state_t state = IDLE;
static timer_t timer = {};
unsigned long time_of_last_state_transition = 0;

// These are provided by the program that includes the state machine
void UART_printf(const char *f, ...);
void set_counter(uint8_t v);
unsigned long millis(void);

void init_state_machine(void)
{
    state = IDLE;
    reset_timer(&timer);
}

void service_state_machine(void)
{
    switch (state)
    {
    case RINGING:
    {
        unsigned long time_in_state = millis() - time_of_last_state_transition;
        if (time_in_state > 2000)
        {
            reset_timer(&timer);
            set_counter(0b000);
            state = IDLE;
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

void step_state(event_t event)
{
    state_t old_state = state;
    switch (state)
    {
    case IDLE:
        switch (event)
        {
        case PRESS:
            state = RUNNING;
            break;
        case CW_ROTATION:
            change_original_timer(&timer, 1);
            UART_printf("%d\n", timer.original_time);
            break;
        case CCW_ROTATION:
            change_original_timer(&timer, -1);
            UART_printf("%d\n", timer.original_time);
            break;
        case LONG_PRESS:
            reset_timer(&timer);
            break;
        default:
            break;
        }
        break;
    case RUNNING:
        switch (event)
        {
        case PRESS:
            state = PAUSED;
            break;
        case SECOND_TICK:
            increment_current_time(&timer);
            UART_printf("%d\n", timer.current_time);
            if (timer_is_finished(&timer))
            {
                state = RINGING;
                UART_printf("Alarm goes off!!!\n");
            }
            break;
        case LONG_PRESS:
            state = IDLE;
            reset_timer(&timer);
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

    if (state == IDLE)
    {
        set_counter(timer.original_time);
    }
    if (state == RUNNING)
    {
        set_counter(timer.current_time);
    }

    if (state != old_state)
    {
        time_of_last_state_transition = millis();
    }
}
