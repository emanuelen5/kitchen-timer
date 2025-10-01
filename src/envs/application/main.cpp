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
#include "serial_commands.h"
#include "max72xx.h"

uint8_queue_t eventQueue;
static const uint8_t queue_buffer_size = 8;
uint8_t event_queue_buffer[queue_buffer_size];

application_t app;
state_machine_t* active_sm = &app.state_machines[app.current_active_sm];

void led_on(void)
{
    UART_printf("led on");
    set_counter(1);
}
void led_off(void)
{
    UART_printf("led off");
    set_counter(0);
}
void version(void)
{
    UART_printf("Kitchen Timer, version 1.0.0\n");
    UART_printf("Authors: Erasmus Cedernaes, Nicolas Perozzi\n");
}

void set_active_timer(uint32_t *steps)
{
    reset_timer(&active_sm->timer);
    set_state(active_sm, SET_TIME);
    change_original_time(&active_sm->timer, (int32_t*)steps);
}

void play_active_timer(void)
{
    if(get_state(active_sm) == SET_TIME)
    {
        state_machine_handle_event(active_sm, SINGLE_PRESS);
    }
    else
    {
        UART_printf("Timer has not SET_TIME mode.\n");
    }
}
void pause_active_timer(void)
{
    if(get_state(active_sm) == RUNNING)
    {
        state_machine_handle_event(active_sm, SINGLE_PRESS);
    }
    else
    {
        UART_printf("Timer has not in RUNNING mode.\n");
    }
}
void reset_active_timer(void)
{
    if(get_state(active_sm) != IDLE)
    {
        state_machine_handle_event(active_sm, LONG_PRESS);
    }
    else
    {
        UART_printf("Timer is in IDLE mode.\n");
    }
}

void convert_seconds_to_hhmmss(uint16_t seconds, uint8_t time[3])
{
    time[0] = seconds / 3600;
    time[1] = (seconds % 3600) / 60;
    time[2] = seconds % 60;
}

void get_status_active_timer(void)
{
    state_t current_state = get_state(active_sm);
    const char* current_state_string = state_to_string(&current_state);

    uint16_t current_seconds = get_current_time(active_sm);
    uint8_t current_time[3];
    convert_seconds_to_hhmmss(current_seconds, current_time);

    uint8_t current_hrs = current_time[0];
    uint8_t current_mins = current_time[1];
    uint8_t current_secs = current_time[2];

    uint16_t original_seconds = get_original_time(active_sm);
    uint8_t original_time[3];
    convert_seconds_to_hhmmss(original_seconds, original_time);

    uint8_t original_hrs = original_time[0];
    uint8_t original_mins = original_time[1];
    uint8_t original_secs = original_time[2];



    UART_printf("Current State: %s\n", current_state_string);
    if(current_state != IDLE && current_state != SET_TIME)
    {
        UART_printf("Timer set to: %02d:%02d:%02d\n", original_hrs, original_mins, original_secs);
        UART_printf("Current time: %02d:%02d:%02d\n", current_hrs, current_mins, current_secs);
    }
    else if(current_state == SET_TIME)
    {
        UART_printf("Timer set to: %02d:%02d:%02d\n", original_hrs, original_mins, original_secs);
    }
}

void setup_brightness(uint8_t *intensity)
{
    if(*intensity > 16)
    {
        UART_printf("The display brightness must be a value between 0 to 15.\n");
        return;
    }

    max72xx_set_intensity(*intensity);
}

void setup_volume(uint8_t *volume)
{
    if(*volume > 11)
    {
        UART_printf("The volume must be a value between 0 and 10.\n");
    }
    app.buzzer.set_volume(*volume);
}

void setup_status(void)
{
    UART_printf("Volume: %d\n", app.buzzer.get_volume());
}


const command_callbacks_t command_callbacks
{
    .led_on = led_on,
    .led_off = led_off,
    .version = version,
    .set_active_timer = set_active_timer,
    .play_active_timer = play_active_timer,
    .pause_active_timer = pause_active_timer,
    .reset_active_timer = reset_active_timer,
    .status_active_timer = get_status_active_timer,
    .setup_brightness = setup_brightness,
    .setup_volume = setup_volume,
    .setup_status = setup_status
};

void on_line_received(char *line) {
    handle_command(line, &command_callbacks);
}

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
    sei();

    while (true)
    {
        service_receive_UART();
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
