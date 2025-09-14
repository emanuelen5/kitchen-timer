#ifndef LED_COUNTER_H
#define LED_COUNTER_H

#include <stdint.h>

void init_hw_led_counter(void);
void reset_led_counter(void);
void decrement_counter(void);
void increment_counter(void);
void set_counter(uint8_t counter);

#endif // LED_COUNTER_H