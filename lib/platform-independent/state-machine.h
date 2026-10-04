#ifndef LIB_STATE_MACHINE_H
#define LIB_STATE_MACHINE_H

#include <stdint.h>
#include "timer.h"
#include "events.h"

typedef enum
{
    SET_TIME,
    RUNNING,
    PAUSED,
    RINGING,
} state_t;

typedef enum
{
    SHOW_TARGET_TIME,
    SHOW_OVERDUE_TIME,
    SHOW_TOTAL_TIME,
    RINGING_DISPLAY_COUNT,
} ringing_display_t;


struct state_machine_t
{
    state_t state;
    state_machine::timer_t timer;
    uint16_t seconds_in_state;
    ringing_display_t ringing_display;
    bool showing_ringing_display_label;
    uint16_t millis_of_ringing_display_change;

    void init();
    void reset();
    void set_state(state_t new_state);
    void set_ringing_display(ringing_display_t new_ringing_display);
    void handle_event(event_t event);
    void service();
    uint16_t get_target_time();
    uint16_t get_time_left();
    uint16_t get_elapsed_time();
    uint16_t get_ringing_display_time();
    state_t get_state();
    bool is_idle();
};

bool is_interactive_event(event_t event);
const char* state_to_string(state_t *state);

#endif // LIB_STATE_MACHINE_H
