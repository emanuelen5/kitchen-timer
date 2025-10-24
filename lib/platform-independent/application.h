#ifndef LIB_APPLICTION_H
#define LIB_APPLICTION_H

#include <stdint.h>
#include "config.h"
#include "state-machine.h"
#include "buzzer.h"
#include "power-save.h"

typedef enum {
    ACTIVE_TIMER_VIEW,
    BARS_VIEW,
    VIEW_COUNT,
    MENU_VIEW,
} application_view_t;

typedef struct
{
    application_view_t current_view;
    state_machine_t state_machines[MAX_TIMERS];
    state_t previous_sm_states[MAX_TIMERS];
    uint8_t current_active_sm;
    Buzzer buzzer;
    PowerSave power_save;
    uint8_t brightness; // [0, 0xf]
} application_t;

void init_application(application_t* app);
void application_handle_event(application_t* app, event_t event);
void service_application(application_t* app);

[[maybe_unused]] static inline const char *application_view_to_string(application_view_t view)
{
    switch (view)
    {
    case ACTIVE_TIMER_VIEW:
        return "ACTIVE TIMER VIEW";
    case BARS_VIEW:
        return "BARS VIEW";
    case VIEW_COUNT:
        return "VIEW COUNT";
    case MENU_VIEW:
        return "MENU VIEW";
    }
    return "UNKNOWN VIEW";
}

#endif // LIB_APPLICTION_H
