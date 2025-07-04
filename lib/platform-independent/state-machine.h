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
    SINGLE_PRESS,
    CW_ROTATION,
    CCW_ROTATION,
    DOUBLE_PRESS,
    LONG_PRESS,
    CW_PRESSED_ROTATION,
    CCW_PRESSED_ROTATION,
    TIMEOUT,
    SECOND_TICK,
} event_t;

class KitchenTimerStateMachine
{
private:
    state_t state;
    uint16_t millis_of_last_transition;
    state_machine::Timer timer;

    void set_state(state_t new_state);

public:
    void init();
    void service();
    void handle_event(event_t event);
    uint16_t get_target_time();
    uint16_t get_current_time();
    state_t get_state();
};

#endif // LIB_STATE_MACHINE_H
