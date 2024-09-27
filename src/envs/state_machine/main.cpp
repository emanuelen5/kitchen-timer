#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "state-machine.h"
#include "led-counter.h"
#include "timer.h"
#include "rotary-encoder.h"
#include "uint8-queue.h"
#include "UART.h"
#include "rtc.h"
#include "millis.h"
#include "util.h"
#include "application.h"

uint8_queue_t eventQueue;
static const uint8_t queue_buffer_size = 8;
uint8_t event_queue_buffer[queue_buffer_size];

application_t app;

void cw_rotation_cb(void)
{
    add_to_queue(&eventQueue, CW_ROTATION);
}

void ccw_rotation_cb(void)
{
    add_to_queue(&eventQueue, CCW_ROTATION);
}

void on_single_press(void)
{
    add_to_queue(&eventQueue, SINGLE_PRESS);
}

void on_double_press(void)
{
    add_to_queue(&eventQueue, DOUBLE_PRESS);
}

void on_long_press(void)
{
    add_to_queue(&eventQueue, LONG_PRESS);
}

void second_tick_cb(void)
{
    add_to_queue(&eventQueue, SECOND_TICK);
}

int main()
{
    Button button(&on_single_press, &on_double_press, &on_long_press);

    init_UART();
    init_timer2_to_1s_interrupt(second_tick_cb);
    init_millis();
    init_led_counter();
    init_queue(&eventQueue, event_queue_buffer, queue_buffer_size);
    init_rotary_encoder(cw_rotation_cb, ccw_rotation_cb, button);
    init_application(&app);
    sei();

    while (true)
    {
        service_receive_UART();
        service_button_press();
        dequeue_return_t event = dequeue(&eventQueue);
        if (event.is_valid)
        {
            step_application(&app, (event_t)event.value);
        }
        service_application(&app);
    }
}
