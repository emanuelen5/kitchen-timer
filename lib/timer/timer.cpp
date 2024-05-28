#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

void change_original_timer(timer_t *timer, int step)
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

bool timer_is_finished(timer_t *timer)
{
    return timer->current_time >= timer->original_time;
}
