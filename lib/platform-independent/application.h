#ifndef LIB_APPLICTION_H
#define LIB_APPLICTION_H

#include <stdint.h>
#include "state-machine.h"

#define MAX_TIMERS 5

typedef struct 
{
    state_machine_t state_machines[MAX_TIMERS];
    uint8_t active_state_machine_index;
} application_t;


#endif // LIB_APPLICTION_H
