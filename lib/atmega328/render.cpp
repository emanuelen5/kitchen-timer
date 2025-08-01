#include <stdint.h>
#include "render.h"
#include "util.h"
#include "config.h"
#include "fat_font.h"
#include "max72xx_matrix.h"
#include "millis.h"

#define FONT_WIDTH 6
#define FONT_HEIGHT 7

void init_render()
{
    matrix_init();
    init_millis();
}

static bool get_blink_state(blink_state_t *state, uint16_t blink_rate)
{
    uint16_t ms = millis();

    if ((ms - state->last_blink_time) >= blink_rate)
    {
        state->last_blink_time = ms;
        state->blink_is_on = !state->blink_is_on;
    }

    return state->blink_is_on;
}

static void draw_timers_indicator(state_machine_t sm[])
{
    for (uint8_t i = 0; i < MAX_TIMERS; i++)
    {
        bool is_set_time = sm[i].state == SET_TIME;
        bool is_running = sm[i].state == RUNNING;
        bool is_paused = sm[i].state == PAUSED;
        bool is_ringing = sm[i].state == RINGING;

        bool show_led = is_set_time || is_running || is_paused || is_ringing;

        matrix_set_pixel(TIMERS_INDICATOR_COLUMN, i, show_led);
    }
}

static void draw_ringing_indicator(state_machine_t sm[])
{
    for (uint8_t i = 0; i < MAX_TIMERS; i++)
    {
        bool is_ringing = sm[i].state == RINGING;
        matrix_set_pixel(RINGING_INDICATOR_COLUMN, i, is_ringing);
    } 
}

static blink_state_t active_timer_blink = {0, true};
void draw_active_timer_indicator(uint8_t active_timer_index)
{
    bool blink_state = get_blink_state(&active_timer_blink, ACTIVE_TIMER_INDICATOR_BLINK_RATE);

    for (uint8_t i = 0; i < MAX_TIMERS; i++)
    {
        if (i == active_timer_index)
        {
            matrix_set_pixel(TIMERS_INDICATOR_COLUMN, i, blink_state);
        }
    }
}

static void draw_digit(char digit, uint8_t x_offset, uint8_t y_offset, bool clear_digit)
{
    const uint8_t* ptr_digit = get_char(digit);
    
    for (uint8_t row = 0; row < FONT_HEIGHT; row++)
    {
        for (uint8_t col = 0; col < FONT_WIDTH; col++)
        {
            bool is_on = false;
            if(!clear_digit)
            {
                is_on = ptr_digit[row] & bit(FONT_WIDTH - 1 - col);  // 6-bit wide
            }
            matrix_set_pixel(x_offset + col, y_offset + row, is_on);
        }
    }
}

static void draw_active_timer(uint16_t current_time, uint8_t x_offset, uint8_t y_offset, bool clear_active_timer)
{
    const bool show_as_hh_mm = current_time >= 3600;
    uint8_t top_value, bottom_value;
    if (show_as_hh_mm) {
        top_value = current_time / 3600;
        bottom_value = (current_time / 60) % 60;
    } else { // mm_ss
        top_value = current_time / 60;
        bottom_value = current_time % 60;
    }


    char top_digits[2], bottom_digits[2];
    top_digits[0] = '0' + (top_value / 10);
    top_digits[1] = '0' + (top_value % 10);

    bottom_digits[0] = '0' + (bottom_value / 10);
    bottom_digits[1] = '0' + (bottom_value % 10);

    
    draw_digit(top_digits[0], x_offset, y_offset, clear_active_timer);
    draw_digit(top_digits[1], x_offset + 7, y_offset, clear_active_timer);

    draw_digit(bottom_digits[0], x_offset, y_offset + 8, clear_active_timer);
    draw_digit(bottom_digits[1], x_offset + 7, y_offset + 8, clear_active_timer);

}

static blink_state_t timer_digits_blink = {0, true};
void render_active_timer_view(state_machine_t* state_machines, uint8_t active_timer_index)
{
    uint16_t time_to_display;
    state_machine_t* active_sm = &state_machines[active_timer_index];
    bool blink = get_blink_state(&timer_digits_blink, TIMER_DIGITS_BLINK_RATE);

    switch(active_sm->state)
    {
        case IDLE:
            time_to_display = 0;
            break;

        case SET_TIME:
            time_to_display = active_sm->timer.original_time;
            break;

        case RINGING:
            
            time_to_display = get_original_time(&active_sm->timer);
            break;
        
        default:
            time_to_display = active_sm->timer.current_time;
            break;
    }

    matrix_buffer_clear();
    draw_timers_indicator(state_machines);
    draw_ringing_indicator(state_machines);
    bool should_blink_timer_numbers = active_sm->state == PAUSED || active_sm->state == RINGING;
    if (should_blink_timer_numbers)
    {
        if(blink)
        {
            draw_active_timer(time_to_display, DIGITS_X_OFFSET, DIGITS_Y_OFFSET, false);
        }
    } else
    {
        draw_active_timer(time_to_display, DIGITS_X_OFFSET, DIGITS_Y_OFFSET, false);
    }
    draw_active_timer_indicator(active_timer_index);
    matrix_update();
}
