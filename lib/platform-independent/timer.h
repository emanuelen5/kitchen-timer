#ifndef LIB_TIMER_H
#define LIB_TIMER_H

#include "stdint.h"
#include "state-machine-types.h"

namespace state_machine
{
    typedef struct
    {
        state_t state;
        uint16_t millis_of_last_transition;
        uint16_t original_time;
        uint16_t current_time;
    } timer_t;

    uint16_t get_original_time(timer_t *timer);
    void change_original_time(timer_t *timer, int step);
    state_t get_state(timer_t *timer);
    void set_state(timer_t *timer, state_t new_state);
    void increment_current_time(timer_t *timer);
    void reset_timer(timer_t *timer);
    bool timer_is_finished(timer_t *timer);
    uint16_t timer_get_seconds_left(timer_t *timer);    

} // namespace state_machine

#endif // LIB_TIMER_H
