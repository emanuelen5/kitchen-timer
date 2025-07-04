#include "state-machine.h"
#include "timer.h"
#include "util.h"

// These are provided by the program that includes the state machine
void UART_printf(const char *f, ...);
void set_counter(uint8_t v);
uint16_t millis(void);

void KitchenTimerStateMachine::init()
{
    this->set_state(IDLE);
    reset_timer(&this->timer);
}

void KitchenTimerStateMachine::set_state(state_t new_state)
{
    this->state = new_state;
}

void KitchenTimerStateMachine::service()
{
    switch (this->state)
    {
    case RINGING:
    {
        uint16_t time_in_state = millis() - this->millis_of_last_transition;
        if (time_in_state >= 2000)
        {
            reset_timer(&this->timer);
            set_counter(0b000);
            this->set_state(IDLE);
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

void KitchenTimerStateMachine::handle_event(event_t event)
{
    switch (this->state)
    {
    case IDLE:
        switch (event)
        {
        case SINGLE_PRESS:
            this->set_state(RUNNING);
            break;
        case CW_ROTATION:
            change_original_time(&this->timer, 1);
            UART_printf("%d\n", this->timer.original_time);
            break;
        case CCW_ROTATION:
            change_original_time(&this->timer, -1);
            UART_printf("%d\n", this->timer.original_time);
            break;
        case LONG_PRESS:
            reset_timer(&this->timer);
            break;
        default:
            break;
        }
        break;
    case RUNNING:
        switch (event)
        {
        case SINGLE_PRESS:
            this->set_state(PAUSED);
            UART_printf("Pause\n");
            break;
        case LONG_PRESS:
            reset_timer(&this->timer);
            this->set_state(IDLE);
            break;
        case SECOND_TICK:
            increment_current_time(&this->timer);
            UART_printf("%d\n", this->timer.current_time);
            if (timer_is_finished(&this->timer))
            {
                this->set_state(RINGING);
                UART_printf("Alarm goes off!!!\n");
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
            this->set_state(RUNNING);
            break;
        case LONG_PRESS:
            reset_timer(&this->timer);
            this->set_state(IDLE);
            break;
        default:
            break;
        }
        break;
    case RINGING:
        switch (event)
        {
        case SINGLE_PRESS:
            reset_timer(&this->timer);
            break;
        default:
            break;
        }
        break;
    }
}

uint16_t KitchenTimerStateMachine::get_original_time()
{
    return this->timer.original_time;
}

uint16_t KitchenTimerStateMachine::get_current_time()
{
    return timer_get_current_time(&this->timer);
}

state_t KitchenTimerStateMachine::get_state()
{
    return this->state;
}
