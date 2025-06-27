#include <avr/interrupt.h>
#include <util/delay.h>
#include "led-counter.h"
#include "util/atomic.h"
#include "millis.h"

#include "util.h"
#include "rtc.h"
#include "uint8-queue.h"
#include "UART.h"

#include "max72xx_matrix.h"
#include "max72xx.h"
#include "SPI.h"
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

void service()
{
    dequeue_return_t event;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        event = dequeue(&eventQueue);
    }

    if (event.is_valid)
    {
        step_application(&app, (event_t)event.value);
    }
    service_application(&app);
    render_timer_view(&app.state_machines[0], 5, 0);
}

int main()
{
    init_led_counter();
    init_millis();
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
    init_SPI(MAX72XX_NUM_DEVICES * 2);
    set_counter(5);

    sei();
    set_counter(6);
    init_max72xx();
    set_counter(7);
    sleep();

    while (true)
    {
        app.state_machines[0].timer.original_time = 10; // Test with 1 minute and 10 secs
        step_application(&app, SINGLE_PRESS);           // To move into RUNNING state
        set_counter(8);
        sleep();

        uint16_t start = millis();

        while (millis() - start < 3000)
        {
            service();
        }
        set_counter(1);
        sleep();

        start = millis();
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            add_to_queue(&eventQueue, SINGLE_PRESS);
        }
        set_counter(2);
        sleep();

        while (millis() - start < 3000)
        {
            service();
        }

        start = millis();
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            add_to_queue(&eventQueue, SINGLE_PRESS);
        }
        set_counter(2);
        sleep();

        while (millis() - start < 3000)
        {
            service();
        }
    }

    return 0;
}
