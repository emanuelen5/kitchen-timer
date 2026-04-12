#include "timer.h"

namespace state_machine
{
    void timer_t::set_current_time_to_target_time()
    {
        this->current_time = this->original_time;
    }

    void timer_t::add_to_target_time(int32_t step)
    {
        int32_t new_time = this->original_time + step;
        new_time = new_time < 0 ? 0 : new_time;
        new_time = new_time > max_time ? max_time : new_time;
        this->original_time = new_time;
    }

    void timer_t::add_to_current_time(int32_t step)
    {
        int32_t new_time = this->current_time + step;
        new_time = new_time < 0 ? 0 : new_time;
        new_time = new_time > max_time ? max_time : new_time;
        this->current_time = new_time;
    }

    void timer_t::reset()
    {
        this->current_time = 0;
        this->original_time = 0;
    }

    void timer_t::decrement_time_left()
    {
        if (this->current_time > 0)
        {
            this->current_time--;
        }
    }

    bool timer_t::is_finished()
    {
        return this->current_time == 0;
    }

    uint16_t timer_t::get_time_left()
    {
        return this->current_time;
    }

    uint16_t timer_t::get_target_time()
    {
        return this->original_time;
    }

} // namespace state_machine
