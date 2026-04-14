#include "state-machine.h"
#include "timer.h"
#include "util.h"
#include "config.h"

// These are provided by the program that includes the state machine
uint16_t millis(void);

void set_state(state_machine_t *sm, state_t new_state)
{
    sm->millis_of_last_transition = millis();
    sm->state = new_state;
}

static void reset_active_state_machine(state_machine_t *sm)
{
    reset_timer(&sm->timer);
    set_state(sm, SET_TIME);
}

bool state_machine_is_idle(state_machine_t *sm)
{
    return sm->state == SET_TIME && sm->timer.original_time == 0;
}

void init_state_machine(state_machine_t *sm)
{
    sm->state = SET_TIME;
    sm->millis_of_last_transition = 0;
    reset_timer(&sm->timer);
}

void service_state_machine(state_machine_t *sm)
{
    switch (sm->state)
    {
    case RINGING:
    {
        uint16_t time_in_ringing_state = millis() - sm->millis_of_last_transition;
        if (time_in_ringing_state >= RINGING_TIMEOUT)
        {
            reset_active_state_machine(sm);
        }
    }
    break;

    default:
        break;
    }
}

typedef enum
{
    ccw,
    cw,
    none,
} rotation_dir_t;

typedef enum
{
    slow,
    fast,
} rotation_speed_t;

static rotation_speed_t event_speed(event_t event)
{
    if (event == CW_ROTATION_FAST || event == CCW_ROTATION_FAST)
    {
        return fast;
    }
    return slow;
}

static rotation_dir_t event_to_rot_dir(event_t event)
{
    if (event == CW_ROTATION || event == CW_ROTATION_FAST)
    {
        return cw;
    }
    else if (event == CCW_ROTATION || event == CCW_ROTATION_FAST)
    {
        return ccw;
    }
    else
    {
        return none;
    }
}

static int16_t get_step_size(uint16_t original_time, rotation_dir_t dir, rotation_speed_t speed)
{
    const uint16_t base_step = (original_time >= 3600) ? 60 : 1;
    const uint16_t snap_size = base_step * 5;

    if (speed == fast)
    {
        uint16_t snapped;
        if (dir == cw)
        {
            // Snap to the strictly next multiple of snap_size above original_time
            snapped = (original_time / snap_size + 1) * snap_size;
        }
        else
        {
            if (original_time == 0)
                return 0;
            // Snap to the strictly previous multiple of snap_size below original_time
            snapped = ((original_time - 1) / snap_size) * snap_size;
        }
        return (int16_t)((int32_t)snapped - (int32_t)original_time);
    }

    int16_t step_size = 0;
    switch (dir)
    {
    case cw:
        step_size = base_step;
        break;
    case ccw:
        step_size = -base_step;
        break;
    default:
        return 0;
    }

    return step_size;
}

void state_machine_handle_event(state_machine_t *sm, event_t event)
{
    switch (sm->state)
    {
    case SET_TIME:
        switch (event)
        {
        case SINGLE_PRESS:
            if(sm->timer.original_time != 0)
            {
                set_current_time_to_target_time(&sm->timer);
                set_state(sm, RUNNING);
            }
            break;

        case CW_ROTATION:
        case CCW_ROTATION:
        case CW_ROTATION_FAST:
        case CCW_ROTATION_FAST:
        {
            const int32_t step_size = get_step_size(sm->timer.original_time, event_to_rot_dir(event), event_speed(event));
            add_to_target_time(&sm->timer, step_size);
        }
        break;

        case LONG_PRESS:
            if(sm->timer.original_time == 0)
            {
                reset_active_state_machine(sm);
            }
            break;

        default:
            // Do nothing
            break;
        }
        break;

    case RUNNING:
        switch (event)
        {
        case SINGLE_PRESS:
            set_state(sm, PAUSED);
            break;

        case CW_ROTATION:
        case CCW_ROTATION:
        case CW_ROTATION_FAST:
        case CCW_ROTATION_FAST:
        {
            const int32_t step_size = get_step_size(sm->timer.original_time, event_to_rot_dir(event), event_speed(event));
            add_to_target_time(&sm->timer, step_size);
            add_to_current_time(&sm->timer, step_size);
        }
        break;

        case LONG_PRESS:
            reset_active_state_machine(sm);
            break;

        case SECOND_TICK:
            decrement_time_left(&sm->timer);
            if (timer_is_finished(&sm->timer))
            {
                set_state(sm, RINGING);
            }
            break;

        default:
            break;
        }
        break;

    case PAUSED:
        switch (event)
        {
        case SINGLE_PRESS:
            set_state(sm, RUNNING);
            break;

        case CW_ROTATION:
        case CCW_ROTATION:
        case CW_ROTATION_FAST:
        case CCW_ROTATION_FAST:
        {
            const int32_t step_size = get_step_size(sm->timer.original_time, event_to_rot_dir(event), event_speed(event));
            add_to_target_time(&sm->timer, step_size);
            add_to_current_time(&sm->timer, step_size);
        }
        break;

        case LONG_PRESS:
            reset_active_state_machine(sm);
            break;

        default:
            break;
        }
        break;

    case RINGING:
        switch (event)
        {
        case SINGLE_PRESS:
        case LONG_PRESS:
            reset_active_state_machine(sm);
            break;

        default:
            break;
        }
        break;
    }
}

uint16_t get_target_time(state_machine_t *sm)
{
    return sm->timer.original_time;
}

uint16_t get_time_left(state_machine_t *sm)
{
    return timer_get_time_left(&sm->timer);
}

state_t get_state(state_machine_t *sm)
{
    return sm->state;
}

bool is_interactive_event(event_t event)
{
    switch (event)
    {
    case SINGLE_PRESS:
    case CW_ROTATION:
    case CCW_ROTATION:
    case CW_ROTATION_FAST:
    case CCW_ROTATION_FAST:
    case DOUBLE_PRESS:
    case LONG_PRESS:
    case CW_PRESSED_ROTATION:
    case CCW_PRESSED_ROTATION:
        return true;
    default:
        return false;
    }
}

const char* state_to_string(state_t *state)
{
    switch (*state)
    {
        case SET_TIME:  return "SET_TIME";
        case RUNNING:   return "RUNNING";
        case PAUSED:    return "PAUSED";
        case RINGING:   return "RINGING";
        default:        return "UNKNOWN_STATE";
    }
}
