#include "timer.h"

namespace state_machine
{

    Timer::Timer() : original_time(0), current_time(0)
    {
    }

    void Timer::increment_target_time(int step)
    {
        int32_t new_time = this->original_time + step;
        new_time = new_time < 0 ? 0 : new_time;
        new_time = new_time > 0xFFFF ? 0xFFFF : new_time;
        this->original_time = new_time;
    }

    void Timer::reset()
    {
        this->current_time = 0;
        this->original_time = 0;
    }

    void Timer::increment_current_time()
    {
        this->current_time++;
    }

    bool Timer::is_expired()
    {
        return this->current_time >= this->original_time;
    }

    uint16_t Timer::get_current_time()
    {
        return this->current_time;
    }

    uint16_t Timer::get_target_time()
    {
        return this->original_time;
    }

} // namespace state_machine
