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

static void draw_timers_indicator(const state_machine_t timers[])
{
    for (uint8_t i = 0; i < MAX_TIMERS; i++)
    {
        bool is_running = timers[i].state == RUNNING;
        bool is_paused = timers[i].state == PAUSED;
        bool is_ringing = timers[i].state == RINGING;

        bool show_led = is_running || is_paused || is_ringing;

        matrix_set_pixel(TIMERS_INDICATOR_COLUMN, i, show_led);
    }

}

void blink_active_timer_indicator(uint8_t active_timer_index)
{
    static uint16_t last_blink_time;
    uint16_t ms = millis();
    static bool blink_state = true;
    const bool should_toggle_active_timer_indicator = (ms - last_blink_time) >= ACTIVE_TIMER_INDICATOR_BLINK_RATE;
    if (should_toggle_active_timer_indicator)
    {
        last_blink_time = ms;
        blink_state = !blink_state;
    }

    for (uint8_t i = 0; i < MAX_TIMERS; i++)
    {
        bool is_active_timer = (i == active_timer_index);
        if( is_active_timer)
        {
            matrix_set_pixel(TIMERS_INDICATOR_COLUMN, i, blink_state);
        }        
    }
    matrix_update();
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
    
    uint8_t top_value, bottom_value;
    if (current_time >= 3600) {
        top_value = current_time / 3600;
        bottom_value = (current_time / 60) % 60;
    } else {
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

/* static void blink_paused_timer(state_machine_t* timers, uint8_t active_timer_index)
{
    static uint16_t last_blink_time;
    uint16_t ms = millis();
    static bool blink_state = true;
    const bool should_toggle_paused_timer = (ms - last_blink_time) >= PAUSED_TIMER_BLINK_RATE;
    if (should_toggle_paused_timer)
    {
        last_blink_time = ms;
        blink_state = !blink_state;
    }

    state_machine_t* active_timer = &timers[active_timer_index];
    uint16_t current_time = active_timer->timer.current_time;
    if(active_timer->state == PAUSED && blink_state)
    {
        draw_active_timer(current_time, DIGITS_X_OFFSET, DIGITS_Y_OFFSET, true);
    }
} */

static bool should_draw_paused_timer(state_machine_t* timers, uint8_t active_timer_index)
{
    static uint16_t last_blink_time;
    static bool blink_state = true;
    uint16_t ms = millis();

    if ((ms - last_blink_time) >= PAUSED_TIMER_BLINK_RATE)
    {
        last_blink_time = ms;
        blink_state = !blink_state;
    }

    state_machine_t* active_timer = &timers[active_timer_index];
    return (active_timer->state != PAUSED) || blink_state;
}

void render_active_timer_view(state_machine_t* timers, uint8_t active_timer_index)
{
    state_machine_t* active_timer = &timers[active_timer_index];
    uint16_t current_time = active_timer->timer.current_time;

    matrix_buffer_clear();
    draw_timers_indicator(timers);
    if (should_draw_paused_timer(timers, active_timer_index))
    {
        draw_active_timer(current_time, DIGITS_X_OFFSET, DIGITS_Y_OFFSET, false);
    }
    matrix_update();
}
