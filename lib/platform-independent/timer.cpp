#include "timer.h"

namespace state_machine
{
    void add_to_target_time(timer_t *timer, int16_t step)
    {
        int32_t new_time = (int32_t) timer->original_time + step;
        new_time = new_time < 0 ? 0 : new_time;
        new_time = new_time > max_time ? max_time : new_time;
        timer->original_time = new_time;
    }

    void reset_timer(timer_t *timer)
    {
        timer->current_time = 0;
        timer->original_time = 0;
    }

    void set_time_left_to_target_time(timer_t *timer)
    {
        timer->current_time = timer->original_time;
    }

    void decrement_time_left(timer_t *timer)
    {
        if (timer->current_time > 0)
        {
            timer->current_time--;
        }
    }

    bool timer_is_finished(timer_t *timer)
    {
        return timer->current_time == 0;
    }

    uint16_t timer_get_time_left(timer_t *timer)
    {
        return timer->current_time;
    }

    uint16_t get_target_time(timer_t * timer)
    {
        return timer->original_time;
    }

} // namespace state_machine
