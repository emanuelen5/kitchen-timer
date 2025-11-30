#ifndef LIB_STATE_MACHINE_H
#define LIB_STATE_MACHINE_H

#include <stdint.h>
#include "timer.h"
#include "events.h"

typedef enum
{
    IDLE,
    SET_TIME,
    RUNNING,
    PAUSED,
    RINGING,
} state_t;


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
const char* state_to_string(state_t *state);

#endif // LIB_STATE_MACHINE_H
