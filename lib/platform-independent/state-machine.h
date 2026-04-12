#ifndef LIB_STATE_MACHINE_H
#define LIB_STATE_MACHINE_H

#include <stdint.h>
#include "timer.h"
#include "events.h"

typedef enum
{
    SET_TIME,
    RUNNING,
    PAUSED,
    RINGING,
} state_t;


struct state_machine_t
{
    state_t state;
    state_machine::timer_t timer;
    uint16_t millis_of_last_transition;

    void init();
    void reset();
    void set_state(state_t new_state);
    void handle_event(event_t event);
    void service();
    uint16_t get_target_time();
    uint16_t get_time_left();
    state_t get_state();
    bool is_idle();
};

bool is_interactive_event(event_t event);
const char* state_to_string(state_t *state);

#endif // LIB_STATE_MACHINE_H
