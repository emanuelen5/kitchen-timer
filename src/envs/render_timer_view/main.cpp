#include <avr/interrupt.h>
#include "render.h"
#include "rtc.h"
#include "uint8-queue.h"
#include "led-counter.h"
#include "millis.h"

uint8_queue_t eventQueue;
static const uint8_t queue_buffer_size = 8;
uint8_t event_queue_buffer[queue_buffer_size];

state_machine_t sm;

void second_tick_cb(void)
{
    add_to_queue(&eventQueue, SECOND_TICK);
}

int main()
{
    init_queue(&eventQueue, event_queue_buffer, queue_buffer_size);
    init_state_machine(&sm);
    init_render();
    init_led_counter();
    init_millis();
    init_timer2_to_1s_interrupt(second_tick_cb);

    sm.timer.original_time = 70; //Test with 1 minute and 10 secs
    step_state(&sm, SINGLE_PRESS); //To move into RUNNING state

    sei();

    while(true)
    {
        dequeue_return_t event = dequeue(&eventQueue);
        if (event.is_valid)
        {
            step_state(&sm, (event_t)event.value);
            increment_counter();
        }
        service_state_machine(&sm);
        render_timer_view(&sm, 1, 0);
    }

    return 0;
}
