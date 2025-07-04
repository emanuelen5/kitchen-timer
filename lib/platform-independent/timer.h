#ifndef LIB_TIMER_H
#define LIB_TIMER_H

#include "stdint.h"

namespace state_machine
{

    class Timer
    {
    private:
        uint16_t original_time;
        uint16_t current_time;

    public:
        Timer();
        void increment_target_time(int step);
        void reset();
        void increment_current_time();
        bool is_expired();
        uint16_t get_current_time();
        uint16_t get_target_time();
    };

} // namespace state_machine

#endif // LIB_TIMER_H
