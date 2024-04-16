#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "led-counter.h"
#include "timer.h"

#define CLK_PIN 2
#define SW_PIN 3
#define DT_PIN 4


typedef enum state      //QUESTION: are we using two names? state and state_t?
{
    IDLE,
    RUNNING,
    PAUSED,
    RINGING,
} state_t;

typedef enum event
{
    PRESS,
    CW_ROTATION,
    CCW_ROTATION,
    DOUBLE_PRESS,
    LONG_PRESS,
    CW_PRESSED_ROTATION,
    CCW_PRESSED_ROTATION,
    TIMEOUT,
    SECOND_TICK,
} event_t;

state_t state = IDLE;
timer_t timer;

void step_state(event_t event);
void init_rotary_encoder(void);

volatile unsigned long last_trigger = 0;
ISR(INT0_vect)
{
    unsigned long t = millis();
    if (t - last_trigger > 70)
    {
        if (PIND & bit(DT_PIN))
            step_state(CW_ROTATION);
        else
            step_state(CCW_ROTATION);
    }
    last_trigger = t;
}

void setup()
{
    reset_timer(&timer);
    init_led_counter();
    init_rotary_encoder();
}

void loop()
{
    
}

void step_state(event_t event)
{
    switch (state)
    {
    case IDLE:
        switch (event)
        {
        case PRESS:
            state = RUNNING;
            break;
        case CW_ROTATION:
            change_timer(&timer, 1);
            break;
        case CCW_ROTATION:
            change_timer(&timer, -1);
            break;
        case LONG_PRESS:
            reset_timer(&timer);
            break;
        }
        break;
    case RUNNING:
        switch (event)
        {
        case PRESS:
            state = PAUSED;
            break;
        case SECOND_TICK:
            decrement_timer(&timer);
            if (timer_is_finished(&timer))
            {
                state = RINGING;
            }
            break;
        case LONG_PRESS:
            state = IDLE;
            reset_timer(&timer);
            break;
        }
        break;
    case PAUSED:
        break;
    case RINGING:
        switch (event)
        {
        case LONG_PRESS:
            /* code */
            break;
        
        default:
            uint8_t count = 0;
            while(count <= 5)
            {
                digitalWrite(A0, HIGH);
                digitalWrite(A1, HIGH);
                digitalWrite(A2, HIGH);
                delay(100);
                digitalWrite(A0, LOW);
                digitalWrite(A1, LOW);
                digitalWrite(A2, LOW);
                delay(100);
                count++;
            }
            reset_timer(&timer);
            state = IDLE;
            break;
        }
        break;
    }

    if (state == IDLE)
    {
        set_counter(timer.original_time);
    }
    if (state == RUNNING)
    {
        set_counter(timer.current_time);
        delay(1000);
    }

/*
    static uint8_t transition = 0;
    set_counter(state | (transition ? bit(2) : 0));
    transition = !transition;
    delay(500);
*/
}

void init_rotary_encoder(void)
{
    DDRD &= 0;
    PORTD |= bit(SW_PIN) | bit(CLK_PIN) | bit(DT_PIN);

    cli();
    EIMSK = bit(INT0);                // Interrupt enable INT0
    EICRA = bit(ISC01) & ~bit(ISC00); // falling interrupt on INT0
    sei();

    increment_counter();
}
