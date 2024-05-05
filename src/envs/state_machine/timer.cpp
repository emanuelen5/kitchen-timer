#include <Arduino.h>

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include "timer.h"

callBack_t second_tick;

void init_timer(callBack_t second_tick_cb)
{
    ASSR |= bit(AS2);       // Timer/Counter2 clocked from external crystal
    TCNT2 = 0;              // Reset Timer/Counter2´s start value
    TCCR2B = bit(CS22);     // 32kHz / 128 prescaler -> Counter = 250Hz 
    while (ASSR & 0x1F)     // Wait for the registers to update
    {
    }
    TIMSK2 = bit(TOIE2);    // Enable Overflow Interrupt in Timer/Counter2 

    second_tick = second_tick_cb;
}


ISR(TIMER2_OVF_vect)
{
    second_tick();
}

void change_timer(timer_t *timer, int step)
{
    int32_t new_time = timer->original_time + step;
    new_time = new_time < 0 ? 0 : new_time;
    new_time = new_time > 0xFFFF ? 0xFFFF : new_time;
    timer->original_time = new_time;
}

void reset_timer(timer_t *timer)
{
    timer->original_time = 180;
}

void decrement_timer(timer_t *timer)
{
    timer->current_time++;
}

bool timer_is_finished(timer_t *timer)
{
    return timer->current_time >= timer->original_time;
}
