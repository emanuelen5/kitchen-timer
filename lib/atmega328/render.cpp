#include "render.h"
#include "fat_font.h"
#include "max72xx_matrix.h"
#include "millis.h"

#define DIGITS_X_OFFSET 2
#define TOP_Y_OFFSET 0
#define BOTTOM_Y_OFFSET 8

static uint16_t last_rendered_time;
static state_t last_rendered_state;
static uint32_t last_pause_blink_time = 0;

static void draw_timers_indicator(const state_machine_t timers[], uint8_t num_timers, uint8_t active_timer_index, uint32_t current_millis)
{
    //TODO
}

static void draw_digit(char digit, uint8_t x_offset, uint8_t y_offset)
{
    //TODO
}

static void draw_active_timer(uint16_t current_time)
{
    //Split the current_time from seconds to MM and SS or HH and MM.
    uint8_t top_value, bottom_value;
    if (current_time >= 3600) {
        top_value = current_time / 3600;
        bottom_value = (current_time / 60) % 60;
    } else {
        top_value = current_time / 60;
        bottom_value = current_time % 60;
    }

    //Add leading zeros if necessary
    char top_digits[2], bottom_digits[2];
    top_digits[0] = '0' + (top_value / 10);
    top_digits[1] = '0' + (top_value % 10);

    bottom_digits[0] = '0' + (bottom_value / 10);
    bottom_digits[1] = '0' + (bottom_value % 10);

    //Draw each digit
    draw_digit(top_digits[0], DIGITS_X_OFFSET, TOP_Y_OFFSET);
    draw_digit(top_digits[1], DIGITS_X_OFFSET + 8, TOP_Y_OFFSET);

    draw_digit(bottom_digits[0], DIGITS_X_OFFSET, BOTTOM_Y_OFFSET);
    draw_digit(bottom_digits[1], DIGITS_X_OFFSET + 8, BOTTOM_Y_OFFSET);

}

static bool is_pause_blink_time_on()
{
    uint16_t millis_now = millis();
    bool pause_blink = (millis_now - last_pause_blink_time) >= 500;
    last_pause_blink_time = millis_now;
    return pause_blink;
}

void render_timer_view(state_machine_t* timers, uint8_t timer_count, uint8_t active_timer_index)
{
    state_machine_t* active_timer = &timers[active_timer_index];
    uint16_t current_time = active_timer->timer.current_time;
    state_t active_timer_state = active_timer->state;

    bool pause_blink_is_on = is_pause_blink_time_on();

    if (current_time == last_rendered_time && active_timer_state == last_rendered_state && pause_blink_is_on)
    {
        return;
    }

    matrix_clear();

    draw_timers_indicator(timers, timer_count, active_timer_index);
    if(active_timer_state != PAUSED && pause_blink_is_on)
    {
        draw_active_timer(current_time);
    }

    matrix_update();

}
