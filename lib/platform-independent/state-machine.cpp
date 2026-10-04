#include "state-machine.h"
#include "timer.h"
#include "util.h"
#include "config.h"

uint16_t millis(void);

void state_machine_t::set_state(state_t new_state)
{
    this->seconds_in_state = 0;
    this->state = new_state;
}

void state_machine_t::set_ringing_display(ringing_display_t new_ringing_display)
{
    this->ringing_display = new_ringing_display;
    this->showing_ringing_display_label = true;
    this->millis_of_ringing_display_change = millis();
}

void state_machine_t::reset()
{
    this->timer.reset();
    this->set_state(SET_TIME);
}

bool state_machine_t::is_idle()
{
    return this->state == SET_TIME && this->timer.original_time == 0;
}

void state_machine_t::init()
{
    this->state = SET_TIME;
    this->seconds_in_state = 0;
    this->timer.reset();
}

void state_machine_t::service()
{
    switch (state)
    {
    case RINGING:
    {
        uint16_t time_since_ringing_display_change = millis() - this->millis_of_ringing_display_change;
        if (time_since_ringing_display_change >= RINGING_DISPLAY_LABEL_DURATION)
        {
            this->showing_ringing_display_label = false;
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
    const int16_t base_step = (original_time >= 3600) ? 60 : 1;

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

    const uint16_t fast_multiplier = 5;
    if (speed == fast)
    {
        step_size *= fast_multiplier;
    }

    return step_size;
}

void state_machine_t::handle_event(event_t event)
{
    if (event == SECOND_TICK && this->seconds_in_state < UINT16_MAX)
    {
        this->seconds_in_state++;
    }

    switch (state)
    {
    case SET_TIME:
        switch (event)
        {
        case SINGLE_PRESS:
            if(this->timer.original_time != 0)
            {
                this->set_state(RUNNING);
            }
            break;

        case CW_ROTATION:
        case CCW_ROTATION:
        case CW_ROTATION_FAST:
        case CCW_ROTATION_FAST:
        {
            const int32_t step_size = get_step_size(this->timer.original_time, event_to_rot_dir(event), event_speed(event));
            this->timer.add_to_target_time(step_size);
        }
        break;

        case LONG_PRESS:
            if(this->timer.original_time == 0)
            {
                this->reset();
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
            this->set_state(PAUSED);
            break;

        case CW_ROTATION:
        case CCW_ROTATION:
        case CW_ROTATION_FAST:
        case CCW_ROTATION_FAST:
        {
            const int32_t step_size = get_step_size(this->timer.original_time, event_to_rot_dir(event), event_speed(event));
            this->timer.add_to_target_time(step_size);
        }
        break;

        case LONG_PRESS:
            this->reset();
            break;

        case SECOND_TICK:
            this->timer.increment_elapsed_time();
            if (this->timer.is_finished())
            {
                this->ringing_display = SHOW_TOTAL_TIME;
                this->showing_ringing_display_label = false;
                this->set_state(RINGING);
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
            this->set_state(RUNNING);
            break;

        case CW_ROTATION:
        case CCW_ROTATION:
        case CW_ROTATION_FAST:
        case CCW_ROTATION_FAST:
        {
            const int32_t step_size = get_step_size(this->timer.original_time, event_to_rot_dir(event), event_speed(event));
            this->timer.add_to_target_time(step_size);
        }
        break;

        case LONG_PRESS:
            this->reset();
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
            this->reset();
            break;
        case CW_ROTATION:
        case CW_ROTATION_FAST:
            this->set_ringing_display((ringing_display_t)((this->ringing_display + 1) % RINGING_DISPLAY_COUNT));
            break;

        case CCW_ROTATION:
        case CCW_ROTATION_FAST:
            this->set_ringing_display((ringing_display_t)((this->ringing_display + RINGING_DISPLAY_COUNT - 1) % RINGING_DISPLAY_COUNT));
            break;

        case SECOND_TICK:
            if (this->seconds_in_state >= RINGING_TIMEOUT)
            {
                this->reset();
            } else {
                this->timer.increment_elapsed_time();
            }
            break;

        default:
            break;
        }
        break;
    }
}

uint16_t state_machine_t::get_target_time()
{
    return this->timer.original_time;
}

uint16_t state_machine_t::get_time_left()
{
    return this->timer.get_time_left();
}

uint16_t state_machine_t::get_elapsed_time()
{
    return this->timer.get_elapsed_time();
}

uint16_t state_machine_t::get_ringing_display_time()
{
    switch (this->ringing_display)
    {
    case SHOW_TARGET_TIME:
        return this->timer.get_target_time();
    case SHOW_OVERDUE_TIME:
        return this->timer.get_overdue_time();
    case SHOW_TOTAL_TIME:
    default:
        return this->timer.get_elapsed_time();
    }
}

state_t state_machine_t::get_state()
{
    return this->state;
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
