#include <avr/io.h>
#include "uint8-queue.h"
#include "util.h"
#include "led-counter.h"

#define TEST_LED PB5

uint8_t buffer[64];
uint8_queue_t test_queue;

void debug_queue(void)
{
    add_to_queue(&test_queue, 0xAA); // Add a test byte
    dequeue_return_t result = dequeue(&test_queue);

    if (result.is_valid && result.value == 0xAA)
    {
        increment_counter(); //Turn on an LED
    }
}

int main(void)
{
    init_hw_led_counter();
    init_queue(&test_queue, buffer, sizeof(buffer));
    debug_queue();
    while (1);
}
