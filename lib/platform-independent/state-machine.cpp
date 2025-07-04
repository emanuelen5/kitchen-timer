#include "state-machine.h"
#include "timer.h"
#include "util.h"

// These are provided by the program that includes the state machine
void UART_printf(const char *f, ...);
void set_counter(uint8_t v);
uint16_t millis(void);

void KitchenTimerStateMachine::reset()
{
    this->set_state(IDLE);
    this->timer.reset();
}

void KitchenTimerStateMachine::set_state(state_t new_state)
{
    this->millis_of_last_transition = millis();
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
            this->timer.reset();
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
            this->timer.increment_target_time(1);
            UART_printf("%d\n", this->get_target_time());
            break;
        case CCW_ROTATION:
            this->timer.increment_target_time(-1);
            UART_printf("%d\n", this->get_target_time());
            break;
        case LONG_PRESS:
            this->timer.reset();
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
            this->timer.reset();
            this->set_state(IDLE);
            break;
        case SECOND_TICK:
            this->timer.increment_current_time();
            UART_printf("%d\n", this->get_current_time());
            if (this->timer.is_expired())
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
            this->timer.reset();
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
            this->timer.reset();
            break;
        default:
            break;
        }
        break;
    }
}

uint16_t KitchenTimerStateMachine::get_target_time()
{
    return this->timer.get_target_time();
}

uint16_t KitchenTimerStateMachine::get_current_time()
{
    return this->timer.get_current_time();
}

state_t KitchenTimerStateMachine::get_state()
{
    return this->state;
}
