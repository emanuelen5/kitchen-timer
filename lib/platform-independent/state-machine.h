#ifndef LIB_STATE_MACHINE_H
#define LIB_STATE_MACHINE_H

#include <stdint.h>
#include "state-machine-types.h"
#include "timer.h"

#define MAX_TIMERS 5

typedef struct
{
    state_machine::timer_t timers[MAX_TIMERS];
    uint8_t current_timer_index;
    uint8_t last_ringing_timer_index;
} state_machine_t;

void step_state(state_machine_t *sm, event_t event);
void init_state_machine(state_machine_t *sm);
void service_state_machine(state_machine_t *sm);
uint16_t get_original_time(state_machine_t *sm);
state_t get_state(state_machine_t *sm);

#endif // LIB_STATE_MACHINE_H
