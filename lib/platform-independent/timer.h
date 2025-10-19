#ifndef LIB_TIMER_H
#define LIB_TIMER_H

#include "stdint.h"

namespace state_machine
{
    constexpr uint16_t max_time = 9 * 3600UL + 59 * 60UL + 59;

    typedef struct
    {
        uint16_t original_time, current_time;
    } timer_t;

    void add_to_target_time(timer_t *timer, int16_t step);
    void reset_timer(timer_t *timer);
    void set_time_left_to_target_time(timer_t *timer);
    void decrement_time_left(timer_t *timer);
    bool timer_is_finished(timer_t *timer);
    uint16_t timer_get_time_left(timer_t *timer);
    uint16_t get_target_time(timer_t * timer);

} // namespace state_machine

#endif // LIB_TIMER_H
