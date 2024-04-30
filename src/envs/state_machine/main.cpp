#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "led-counter.h"
#include "timer.h"
#include "rotary-encoder.h"
#include "event_queue.h"

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
event_queue_t eventQueue;

void step_state(uint8_t event);

void cw_rotation_cb(void)
{
    queue_event(&eventQueue, CW_ROTATION);
}

void ccw_rotation_cb(void)
{
    queue_event(&eventQueue, CCW_ROTATION);
}

void setup()
{
    reset_timer(&timer);
    init_led_counter();
    init_queue(&eventQueue);
    init_rotary_encoder(cw_rotation_cb, ccw_rotation_cb);
}

void loop()
{
    value_t dequeue_value = dequeue_event(&eventQueue);
    if (dequeue_value.is_valid)
    {
        step_state(dequeue_value.value);
    }
}

void step_state(uint8_t event)
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
        switch (event)
        {
        case LONG_PRESS:
            /* code */
            break;

        default:
            uint8_t count = 0;
            while (count <= 5)
            {
                digitalWrite(A0, HIGH);
                digitalWrite(A1, HIGH);
                digitalWrite(A2, HIGH);
                delay(100);
                digitalWrite(A0, LOW);
                digitalWrite(A1, LOW);
                digitalWrite(A2, LOW);
                delay(100);
                count++;
            }
            reset_timer(&timer);
            state = IDLE;
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
        delay(1000);
    }
}
