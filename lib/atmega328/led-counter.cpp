#include "led-counter.h"
#include <avr/io.h>
#include "util.h"

volatile static uint8_t count;
#define LED_MASK 0b111

void init_led_counter(void)
{
    DDRC |= LED_MASK;
    PORTC &= ~LED_MASK;
    reset_led_counter();
}

void update_leds()
{
    uint8_t zeroed_port = (PORTC & ~LED_MASK);
    // The diodes are placed upside down, so here we invert the bits
    uint8_t mapped_count = ((count & bit(0)) << 2) | (count & bit(1)) | ((count & bit(2)) >> 2);
    uint8_t masked_count = (LED_MASK & mapped_count);
    PORTC = zeroed_port | masked_count;
}

void reset_led_counter(void)
{
    count = 0;
    update_leds();
}

void decrement_counter(void)
{
    count--;
    update_leds();
}

void increment_counter(void)
{
    count++;
    update_leds();
}

void set_counter(uint8_t counter){
    count = counter;
    update_leds();
}


