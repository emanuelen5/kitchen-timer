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

void single_button_press_cb(void)
{
    add_to_queue(&eventQueue, SINGLE_PRESS);
}

void double_button_press_cb(void)
{
    add_to_queue(&eventQueue, DOUBLE_PRESS);
}

void long_button_press_cb(void)
{
    add_to_queue(&eventQueue, LONG_PRESS);
}

void second_tick_cb(void)
{
    add_to_queue(&eventQueue, SECOND_TICK);
}

int main()
{
    init_UART();
    init_timer2_to_1s_interrupt(second_tick_cb);
    init_millis();
    init_led_counter();
    init_queue(&eventQueue, event_queue_buffer, queue_buffer_size);
    init_rotary_encoder(cw_rotation_cb, ccw_rotation_cb, single_button_press_cb, double_button_press_cb, long_button_press_cb);
    init_application(&app);
    sei();

    while (true)
    {
        service_receive_UART();
        service_button_press();
        service_state_machine(&app.state_machines[app.active_state_machine_index]);
        dequeue_return_t event = dequeue(&eventQueue);
        if (event.is_valid)
        {
            step_application(&app, (event_t)event.value);
            step_state(&app.state_machines[app.active_state_machine_index], (event_t)event.value);
        }
        
    }
}
