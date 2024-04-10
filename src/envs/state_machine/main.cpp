#include <Arduino.h>
#include "led-counter.h"
#include "timer.h"

typedef enum state
{
    IDLE,
    RUNNING,
    PAUSED,
    RINGING,
} state_t;

typedef enum event
{
    PRESS,
    CW_ROTATION,
    CCW_ROTATION,
    DOUBLE_PRESS,
    LONG_PRESS,
    CW_PRESSED_ROTATION,
    CCW_PRESSED_ROTATION,
    TIMEOUT,
    SECOND_TICK,
} event_t;

state_t state = IDLE;
timer_t timer;
void step_state(event_t event);

void setup()
{
    reset_timer(&timer);
    init_led_counter();
}

void loop()
{
    step_state(PRESS);
    step_state(PRESS);
    step_state(PRESS);
    step_state(PRESS);
}

void step_state(event_t event)
{
    switch (state)
    {
    case IDLE:
        switch (event)
        {
        case PRESS:
            state = RUNNING;
            break;
        case CW_ROTATION:
            change_timer(&timer, 1);
            break;
        case CCW_ROTATION:
            change_timer(&timer, -1);
            break;
        case LONG_PRESS:
            reset_timer(&timer);
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
            decrement_timer(&timer);
            if (timer_is_finished(&timer))
            {
                state = RINGING;
            }
            break;
        case LONG_PRESS:
            state = IDLE;
            reset_timer(&timer);
            break;
        }
        break;
    case PAUSED:
        break;
    case RINGING:
        break;
    }

    static uint8_t transition = 0;
    set_counter(state | (transition ? bit(2) : 0));
    transition = !transition;
    delay(500);
}
