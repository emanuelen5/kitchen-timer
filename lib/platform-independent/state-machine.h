#ifndef LIB_STATE_MACHINE_H
#define LIB_STATE_MACHINE_H

#include <stdint.h>
#include "timer.h"

typedef enum
{
    IDLE,
    SET_TIME,
    RUNNING,
    PAUSED,
    RINGING,
} state_t;

typedef enum
{
    SINGLE_PRESS,
    CW_ROTATION,
    CCW_ROTATION,
    CW_ROTATION_FAST,
    CCW_ROTATION_FAST,
    DOUBLE_PRESS,
    LONG_PRESS,
    CW_PRESSED_ROTATION,
    CCW_PRESSED_ROTATION,
    SECOND_TICK,
} event_t;

typedef struct
{
    state_t state;
    state_t prev_state;
    state_machine::timer_t timer;
    uint16_t millis_of_last_transition;
} state_machine_t;

void set_state(state_machine_t *sm, state_t new_state);
void state_machine_handle_event(state_machine_t *sm, event_t event);
void init_state_machine(state_machine_t *sm);
void service_state_machine(state_machine_t *sm);
uint16_t get_target_time(state_machine_t *sm);
uint16_t get_time_left(state_machine_t *sm);
state_t get_state(state_machine_t *sm);
bool is_interactive_event(event_t event);

[[maybe_unused]] inline const char *sm_state_to_string(state_t state)
{
    switch (state)
    {
    case IDLE:
        return "IDLE";
    case SET_TIME:
        return "SET_TIME";
    case RUNNING:
        return "RUNNING";
    case PAUSED:
        return "PAUSED";
    case RINGING:
        return "RINGING";
    default:
        return "UNKNOWN";
    }
}

#endif // LIB_STATE_MACHINE_H
