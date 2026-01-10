#ifndef LIB_APPLICTION_H
#define LIB_APPLICTION_H

#include <stdint.h>
#include "config.h"
#include "events.h"
#include "state-machine.h"
#include "settings_menu.h"
#include "buzzer.h"
#include "power-save.h"

typedef enum {
    ACTIVE_TIMER_VIEW,
    SETTINGS_MENU_VIEW,
    BRIGHTNESS_SETTING_VIEW,
    VIEW_COUNT
} application_view_t;

typedef struct
{
    application_view_t current_view;
    state_machine_t state_machines[MAX_TIMERS];
    state_t previous_sm_states[MAX_TIMERS];
    uint8_t current_active_sm;
    settings_menu_t settings_menu;
    Buzzer buzzer;
    PowerSave power_save;
    uint8_t brightness; // [0, 0xf]
} application_t;

void init_application(application_t* app);
void application_handle_event(application_t* app, event_t event);
void service_application(application_t* app);

#endif // LIB_APPLICTION_H
