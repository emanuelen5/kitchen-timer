#ifndef LIB_STATE_MACHINE_H
#define LIB_STATE_MACHINE_H

#include <stdint.h>
#include "timer.h"

typedef enum
{
    IDLE,
    RUNNING,
    PAUSED,
    RINGING,
} state_t;

typedef enum
{
    PRESS,
    CW_ROTATION,
    CCW_ROTATION,
    DOUBLE_PRESS,
    LONG_PRESS,
    CW_PRESSED_ROTATION,
    CCW_PRESSED_ROTATION,
    TIMEOUT,
    SECOND_TICK,
} event_t;

typedef struct
{
    state_t state;
    state_machine::timer_t timer;
    uint16_t millis_of_last_transition;
} state_machine_t;

void step_state(state_machine_t *sm, event_t event);
void init_state_machine(state_machine_t *sm);
void service_state_machine(state_machine_t *sm);
uint16_t get_original_time(state_machine_t *sm);
state_t get_state(state_machine_t *sm);

#endif // LIB_STATE_MACHINE_H
