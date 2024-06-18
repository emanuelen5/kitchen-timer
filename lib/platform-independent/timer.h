#ifndef LIB_TIMER_H
#define LIB_TIMER_H

#include "stdint.h"

namespace state_machine
{

    typedef struct
    {
        uint16_t original_time, current_time;
    } timer_t;

    void change_original_time(timer_t *timer, int step);
    void reset_timer(timer_t *timer);
    void increment_current_time(timer_t *timer);
    bool timer_is_finished(timer_t *timer);
    uint16_t timer_get_seconds_left(timer_t *timer);

} // namespace state_machine

#endif // LIB_TIMER_H
