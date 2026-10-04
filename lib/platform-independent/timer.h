#ifndef LIB_TIMER_H
#define LIB_TIMER_H

#include "stdint.h"

namespace state_machine
{
    constexpr uint16_t max_time = 9 * 3600UL + 59 * 60UL + 59;

    struct timer_t
    {
        uint16_t original_time, elapsed_time;

        void add_to_target_time(int32_t step);
        void reset();
        void increment_elapsed_time();
        bool is_finished();
        uint16_t get_time_left();
        uint16_t get_elapsed_time();
        uint16_t get_target_time();
    };

} // namespace state_machine

#endif // LIB_TIMER_H
