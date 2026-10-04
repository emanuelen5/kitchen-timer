#include "timer.h"

namespace state_machine
{
    void timer_t::add_to_target_time(int32_t step)
    {
        int32_t new_time = this->original_time + step;
        new_time = new_time < 0 ? 0 : new_time;
        new_time = new_time > max_time ? max_time : new_time;
        this->original_time = new_time;
    }

    void timer_t::reset()
    {
        this->elapsed_time = 0;
        this->original_time = 0;
    }

    void timer_t::increment_elapsed_time()
    {
        if (this->elapsed_time < max_time)
        {
            this->elapsed_time++;
        }
    }

    bool timer_t::is_finished()
    {
        return this->elapsed_time >= this->original_time;
    }

    uint16_t timer_t::get_time_left()
    {
        return this->is_finished() ? 0 : this->original_time - this->elapsed_time;
    }

    uint16_t timer_t::get_elapsed_time()
    {
        return this->elapsed_time;
    }

    uint16_t timer_t::get_overdue_time()
    {
        return this->is_finished() ? this->elapsed_time - this->original_time : 0;
    }

    uint16_t timer_t::get_target_time()
    {
        return this->original_time;
    }

} // namespace state_machine
