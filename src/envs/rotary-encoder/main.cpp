#include <Arduino.h>
#include "led-counter.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define CLK_PIN 2
#define SW_PIN 3
#define DT_PIN 4

volatile unsigned long last_trigger = 0;

ISR(INT0_vect)
{
    unsigned long t = millis();
    if (t - last_trigger > 70)
    {
        if (PIND & bit(DT_PIN))
            decrement_counter();
        else
            increment_counter();
    }
    last_trigger = t;
}

void setup()
{
    init_led_counter();

    DDRD &= 0;
    PORTD |= bit(SW_PIN) | bit(CLK_PIN) | bit(DT_PIN);

    cli();
    EIMSK = bit(INT0);                // Interrupt enable INT0
    EICRA = bit(ISC01) & ~bit(ISC00); // falling interrupt on INT0
    sei();

    increment_counter();
}

void loop()
{
    if ((PIND & bit(SW_PIN)) == 0)
    {
        reset_led_counter();
    }
}
