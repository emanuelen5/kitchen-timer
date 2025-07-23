#include <avr/io.h>
#include <avr/interrupt.h>

#include "util.h"
#include "rtc.h"
#include "uint8-queue.h"
#include "UART.h"
#include "millis.h"
#include "render.h"
#include "application.h"
#include "led-counter.h"
#include "avr_button.h"
#include "rotary-encoder.h"

uint8_queue_t eventQueue;
static const uint8_t queue_buffer_size = 8;
uint8_t event_queue_buffer[queue_buffer_size];
bool is_paused = false;

application_t app;

void rotation_cb(rotation_dir_t dir, bool held_down)
{
    UNUSED held_down;
    if (dir == cw)
        add_to_queue(&eventQueue, CW_ROTATION);
    else if (dir == ccw)
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
    AvrButton button(&on_single_press, &on_double_press, &on_long_press);

    init_UART();
    init_timer2_to_1s_interrupt(&second_tick_cb);
    init_millis();
    init_led_counter();
    init_queue(&eventQueue, event_queue_buffer, queue_buffer_size);
    init_rotary_encoder(rotation_cb, button);
    init_application(&app);
    init_render();
    sei();
    
    while (true)
    {
        service_receive_UART();
        button.service();
        dequeue_return_t event = dequeue(&eventQueue);
        if (event.is_valid)
        {
            application_handle_event(&app, (event_t)event.value);
        }
        service_application(&app);
        render_active_timer_view(app.state_machines, app.current_active_sm);
    }

    return 0;
}
