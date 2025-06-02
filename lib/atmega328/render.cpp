#include "render.h"

#define DIGITS_X_OFFSET 2
#define TOP_Y_OFFSET 0
#define BOTTOM_Y_OFFSET 8
#define ACTIVE_INDICATOR_BLINKING_RATE 500
#define PAUSED_TIMER_BLINKING_RATE 500
#define TIMERS_INDICATOR_COLUMN 0
#define FONT_WIDTH 6
#define FONT_HEIGHT 7

static uint16_t last_rendered_time;
static state_t last_rendered_state;
static uint32_t last_pause_blink_time = 0;

uint16_t millis(void);

void init_render()
{
    matrix_init();
}

static void draw_timers_indicator(const state_machine_t timers[], uint8_t num_timers, uint8_t active_timer_index)
{
    for (uint8_t i = 0; i < num_timers && i < MAX_TIMERS; i++)
    {
        bool is_idle = timers[i].state == IDLE;
        bool is_running = timers[i].state == RUNNING;
        bool is_paused = timers[i].state == PAUSED;
        bool is_ringing = timers[i].state == RINGING;
        bool is_active = (i == active_timer_index);

        bool show_led = is_idle || is_running || is_paused || is_ringing;

        if (is_active) {
            if ((millis() / ACTIVE_INDICATOR_BLINKING_RATE) % 2 == 0) {
                show_led = true;  // blink at 1 Hz
            } else {
                show_led = false;
            }
        }
        matrix_set_pixel(TIMERS_INDICATOR_COLUMN, i, show_led);
    }

}

static void draw_digit(char digit, uint8_t x_offset, uint8_t y_offset)
{
    const uint8_t* ptr_digit = get_char(digit);
    
    for (uint8_t row = 0; row < FONT_HEIGHT; row++)
    {
        for (uint8_t col = 0; col < FONT_WIDTH; col++)
        {
            bool is_on = ptr_digit[row] & (1 << (5 - col));  // 6-bit wide
            matrix_set_pixel(x_offset + col, y_offset + row, is_on);
        }
    }
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
    bool pause_blink = (millis_now - last_pause_blink_time) >= PAUSED_TIMER_BLINKING_RATE;
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
    if(active_timer_state != PAUSED || pause_blink_is_on)
    {
        draw_active_timer(current_time);
    }

    matrix_update();

    last_rendered_time = current_time;
    last_rendered_state = active_timer_state;

}
