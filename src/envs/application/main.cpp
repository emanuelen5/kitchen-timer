#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

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
#include "serial_commands.h"
#include "max72xx.h"

uint8_queue_t eventQueue;
static const uint8_t queue_buffer_size = 8;
uint8_t event_queue_buffer[queue_buffer_size];

application_t app;
state_machine_t* active_sm = &app.state_machines[app.current_active_sm];


void rotation_cb(rotation_dir_t dir, rotation_speed_t speed, bool held_down)
{
    if (held_down)
    {
        if(dir == cw)
        {
            add_to_queue(&eventQueue, CW_PRESSED_ROTATION);
        }
        else if (dir == ccw)
        {
            add_to_queue(&eventQueue, CCW_PRESSED_ROTATION);
        }
    }
    else
    {
        if (speed == fast)
        {
            if (dir == cw)
            {
                add_to_queue(&eventQueue, CW_ROTATION_FAST);
            }
            else if (dir == ccw)
            {
                add_to_queue(&eventQueue, CCW_ROTATION_FAST);
            }
        } else
        {
            if (dir == cw)
            {
                add_to_queue(&eventQueue, CW_ROTATION);
            }
            else if (dir == ccw)
            {
                add_to_queue(&eventQueue, CCW_ROTATION);
            }
        }
    }
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


void on_line_received(char *line)
{
    handle_command(line, &command_callbacks, &app);
}

int main()
{
    AvrButton button(&on_single_press, &on_double_press, &on_long_press);

    init_hw_UART(on_line_received);
    init_hw_timer2_to_1s_interrupt(&second_tick_cb);
    init_hw_millis();
    init_hw_led_counter();
    init_hw_max72xx();
    init_queue(&eventQueue, event_queue_buffer, queue_buffer_size);
    init_hw_rotary_encoder(rotation_cb, button);
    init_application(&app);
    max72xx_set_intensity(app.brightness);
    sei();

    init_max72xx();

    while (true)
    {
        for (uint8_t count = UART_received_char_count(); count != 0; count--)
        {
            service_receive_UART();
        }
        button.service();
        dequeue_return_t event;
        while ((event = dequeue(&eventQueue)).is_valid)
        {
            application_handle_event(&app, (event_t)event.value);
        }
        service_application(&app);
        render_active_timer_view(app.state_machines, app.current_active_sm);
    }

    return 0;
}
