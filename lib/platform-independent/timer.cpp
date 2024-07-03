#include "timer.h"
#include "millis.h"

namespace state_machine
{

    void change_original_time(timer_t *timer, int step)
    {
        int32_t new_time = timer->original_time + step;
        new_time = new_time < 0 ? 0 : new_time;
        new_time = new_time > 0xFFFF ? 0xFFFF : new_time;
        timer->original_time = new_time;
    }

    void reset_timer(timer_t *timer)
    {
        timer->state = IDLE;
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

    void set_state(timer_t *timer, state_t new_state)
    {
        timer->millis_of_last_transition = millis();
        timer->state = new_state;
    }

    uint16_t get_original_time(timer_t *timer)
    {
        return timer->original_time;
    }

    state_t get_state(timer_t *timer)
    {
        return timer->state;
    }

} // namespace state_machine
