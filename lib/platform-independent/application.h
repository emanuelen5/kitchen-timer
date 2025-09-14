#ifndef LIB_APPLICTION_H
#define LIB_APPLICTION_H

#include <stdint.h>
#include "config.h"
#include "state-machine.h"
#include "buzzer.h"

typedef enum {
    ACTIVE_TIMER_VIEW,
    BARS_VIEW,
    VIEW_COUNT
} application_view_t;

typedef struct
{
    application_view_t current_view;
    state_machine_t state_machines[MAX_TIMERS];
    state_t previous_sm_states[MAX_TIMERS];
    uint8_t current_active_sm;
    Buzzer buzzer;
} application_t;

void init_application(application_t* app);
void application_handle_event(application_t* app, event_t event);
void service_application(application_t* app);

#endif // LIB_APPLICTION_H
