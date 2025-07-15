#ifndef LIB_APPLICTION_H
#define LIB_APPLICTION_H

#include <stdint.h>
#include "config.h"
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
    uint8_t active_sm_index;
    state_machine_t* active_sm = state_machines[active_sm_index];
} application_t;

void init_application(application_t* app);
void application_handle_event(application_t* app, event_t event);
void service_application(application_t* app);

#endif // LIB_APPLICTION_H
