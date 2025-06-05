#include <avr/io.h>
#include <avr/interrupt.h>

#include "util.h"
#include "rtc.h"
#include "uint8-queue.h"
#include "UART.h"

#include "render.h"
#include "led-counter.h"

uint8_queue_t eventQueue;
static const uint8_t queue_buffer_size = 8;
uint8_t event_queue_buffer[queue_buffer_size];
bool is_paused = false;

state_machine_t sm;

void second_tick_cb(void)
{
    add_to_queue(&eventQueue, SECOND_TICK);
}

int main()
{
    init_led_counter();
    init_UART();

    init_queue(&eventQueue, event_queue_buffer, queue_buffer_size);
    init_timer2_to_1s_interrupt(&second_tick_cb);
    init_state_machine(&sm);
    init_render();

    sei();
    
    sm.timer.original_time = 10; //Test with 1 minute and 10 secs
    step_state(&sm, SINGLE_PRESS); //To move into RUNNING state

    while (true)
    {
        uint16_t millis_now = millis();

        if(!is_paused && millis_now >= 3001)
        {
            step_state(&sm, SINGLE_PRESS); // Simulate PAUSE
            is_paused = true;
        }

        if(is_paused && millis_now >= 7001)
        {
            step_state(&sm, SINGLE_PRESS);
            is_paused = false;
        }


        dequeue_return_t event = dequeue(&eventQueue);
        if (event.is_valid)
        {
            step_state(&sm, (event_t)event.value);
            //increment_counter();
        }
        service_state_machine(&sm);
        render_timer_view(&sm, 5, 0);
    }

    return 0;
}
