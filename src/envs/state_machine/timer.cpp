#include <Arduino.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

callBack_t second_tick;

void init_timer(timer_t* timer, callBack_t second_tick_cb)
{
    ASSR |= bit(AS2);       // Timer/Counter2 clocked from external crystal
    TCCR2A |= bit(WGM21);   // Set CTC mode on Timer/Counter2
    TCCR2B |= bit(CS22);     // 32kHz / 128 prescaler -> Counter = 250Hz 
    while (ASSR & 0x1F)     // Wait for the registers to update
    {
    }
    TIMSK2 |= bit(OCIE2A);    // Enable Output Compare Interrupt in Timer/Counter2 buffer A 
    TCNT2 = 0;              // Reset Timer/Counter2´s start value
    OCR2A = 250;             

    reset_timer(timer);

    second_tick = second_tick_cb;
}


ISR(TIMER2_COMPA_vect)
{
    second_tick();
}

void change_original_time(timer_t *timer, int step)
{
    int32_t new_time = timer->original_time + step;
    new_time = new_time < 0 ? 0 : new_time;
    new_time = new_time > 0xFFFF ? 0xFFFF : new_time;
    timer->original_time = new_time;
}

void reset_timer(timer_t *timer)
{
    timer->current_time = 0;
    timer->original_time = 0;
}

void increment_current_time(timer_t *timer)
{
    timer->current_time++;
}

bool current_time_is_finished(timer_t *timer)
{
    return timer->current_time >= timer->original_time;
}
