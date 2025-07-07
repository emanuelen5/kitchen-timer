#ifndef LIB_APPLICTION_H
#define LIB_APPLICTION_H

#include <stdint.h>
#include "state-machine.h"

#define MAX_TIMERS 5

typedef struct
{
    KitchenTimerStateMachine state_machines[MAX_TIMERS];
    uint8_t active_state_machine_index;
} application_t;

void init_application(application_t *app);
void step_application(application_t *app, event_t event);
void service_application(application_t *app);

#endif // LIB_APPLICTION_H
