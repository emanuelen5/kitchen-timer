#ifndef LIB_STATE_MACHINE_TYPES_H
#define LIB_STATE_MACHINE_TYPES_H

#include <stdint.h>

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

#endif //LIB_STATE_MACHINE_TYPES_H
