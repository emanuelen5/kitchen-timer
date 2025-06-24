#include <avr/interrupt.h>
#include <util/delay.h>
#include "led-counter.h"

#include "util.h"
#include "rtc.h"
#include "uint8-queue.h"
#include "UART.h"

#include "render.h"
#include "application.h"

uint8_queue_t eventQueue;
static const uint8_t queue_buffer_size = 8;
uint8_t event_queue_buffer[queue_buffer_size];
bool is_paused = false;

state_machine_t sm;
application_t app;

static void second_tick_cb(void)
{
    add_to_queue(&eventQueue, SECOND_TICK);
}

static void sleep(void)
{
    _delay_ms(100);
}

int main()
{
    init_led_counter();
    set_counter(0);
    sleep();
    init_UART();
    set_counter(1);
    sleep();
    init_queue(&eventQueue, event_queue_buffer, queue_buffer_size);
    set_counter(2);
    sleep();
    init_timer2_to_1s_interrupt(&second_tick_cb);
    set_counter(3);
    sleep();
    init_application(&app);
    set_counter(4);
    sleep();
    init_render();
    set_counter(5);
    sleep();

    app.state_machines[0].timer.original_time = 10; // Test with 1 minute and 10 secs
    step_application(&app, SINGLE_PRESS);           // To move into RUNNING state
    set_counter(6);
    sleep();

    sei();

    while (true)
    {
        // uint16_t millis_now = millis();
        increment_counter();
        sleep();

        // if (!is_paused && millis_now >= 3000)
        // {
        //     step_application(&app, SINGLE_PRESS); // Simulate PAUSE
        //     is_paused = true;
        // }

        // if (is_paused && millis_now >= 7000)
        // {
        //     step_application(&app, SINGLE_PRESS);
        //     is_paused = false;
        // }

        dequeue_return_t event = dequeue(&eventQueue);
        if (event.is_valid)
        {
            step_application(&app, (event_t)event.value);
        }
        service_application(&app);
        render_timer_view(&app.state_machines[0], 5, 0);
    }

    return 0;
}
