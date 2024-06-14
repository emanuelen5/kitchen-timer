#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "led-counter.h"
#include "timer.h"
#include "rotary-encoder.h"
#include "uint8-queue.h"
#include "UART.h"
#include "rtc.h"
#include "millis.h"
#include "util.h"

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
uint8_queue_t eventQueue;
static const uint8_t queue_buffer_size = 8;
uint8_t event_queue_buffer[queue_buffer_size];
uint16_t time_of_last_state_transition = 0;

void cw_rotation_cb(void)
{
    add_to_queue(&eventQueue, CW_ROTATION);
}

void ccw_rotation_cb(void)
{
    add_to_queue(&eventQueue, CCW_ROTATION);
}

void button_press_cb(void)
{
    add_to_queue(&eventQueue, PRESS);
}

void second_tick_cb(void)
{
    add_to_queue(&eventQueue, SECOND_TICK);
}

static void service_state_machine(void)
{
    switch (state)
    {
    case RINGING:
    {
        uint16_t time_in_state = millis() - time_of_last_state_transition;
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

int main()
{
    init_UART();
    init_timer2_to_1s_interrupt(second_tick_cb);
    init_millis();
    init_led_counter();
    init_queue(&eventQueue, event_queue_buffer, queue_buffer_size);
    init_rotary_encoder(cw_rotation_cb, ccw_rotation_cb, button_press_cb);
    reset_timer(&timer);
    sei();

    while (true)
    {
        service_receive_UART();

        dequeue_return_t event = dequeue(&eventQueue);
        if (event.is_valid)
        {
            step_state((event_t)event.value);
        }
        service_state_machine();
    }
}
