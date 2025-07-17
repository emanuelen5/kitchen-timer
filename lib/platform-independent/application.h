#ifndef LIB_APPLICTION_H
#define LIB_APPLICTION_H

#include <stdint.h>
#include "config.h"
#include "state-machine.h"

typedef struct 
{
    state_machine_t state_machines[MAX_TIMERS];
    uint8_t active_state_machine_index;
} application_t;

void init_application(application_t *app);
void step_application(application_t *app, event_t event);
void service_application(application_t *app);

#endif // LIB_APPLICTION_H
