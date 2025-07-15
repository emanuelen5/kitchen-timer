#ifndef LIB_APPLICTION_H
#define LIB_APPLICTION_H

#include <stdint.h>
#include "state-machine.h"

#define MAX_TIMERS 5       //TODO: This goes in config.h

typedef enum {
    ACTIVE_TIMER_VIEW,
    BARS_VIEW
} application_view;

typedef struct 
{
    application_view current_view;
    state_machine_t* state_machines[MAX_TIMERS];
    uint8_t sm_count;
    state_machine_t* active_sm;
    uint8_t active_sm_index;
} application_t;

void init_application(application_t *app);
void step_application(application_t *app, event_t event);
void service_application(application_t *app);

#endif // LIB_APPLICTION_H
