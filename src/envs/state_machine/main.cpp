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

uint8_queue_t eventQueue;
static const uint8_t queue_buffer_size = 8;
uint8_t event_queue_buffer[queue_buffer_size];

state_machine_t sm;

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

void second_tick(void)
{
    add_to_queue(&eventQueue, SECOND_TICK);
}

int main()
{
    init_UART();
    init_timer2_to_1s_interrupt(second_tick);
    init_millis();
    init_led_counter();
    init_queue(&eventQueue, event_queue_buffer, queue_buffer_size);
    init_rotary_encoder(cw_rotation_cb, ccw_rotation_cb, button_press_cb);
    init_state_machine(&sm);
    sei();

    while (true)
    {
        service_receive_UART();

        dequeue_return_t event = dequeue(&eventQueue);
        if (event.is_valid)
        {
            step_state(&sm, (event_t)event.value);
        }
        service_state_machine(&sm);
    }
}
