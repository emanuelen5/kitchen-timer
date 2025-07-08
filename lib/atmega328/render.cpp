#include "render.h"

#define DIGITS_X_OFFSET 2
#define TOP_Y_OFFSET 0
#define BOTTOM_Y_OFFSET 8
#define ACTIVE_INDICATOR_BLINK_RATE 200
#define PAUSED_TIMER_BLINK_RATE 500
#define TIMERS_INDICATOR_COLUMN 0
#define FONT_WIDTH 6
#define FONT_HEIGHT 7

static uint16_t last_rendered_time;
static state_t last_rendered_state;
static bool last_active_timer_indicator_blink_is_on;
static bool last_paused_timer_blink_is_on;

void init_render()
{
    matrix_init();
    init_millis();
}

static bool is_active_timer_indicator_blink_on()
{
    static bool is_on = true;
    uint16_t millis_now = millis();
    static uint16_t last_active_timer_indicator_blink_time;

    if ((millis_now - last_active_timer_indicator_blink_time) >= ACTIVE_INDICATOR_BLINK_RATE) {
        last_active_timer_indicator_blink_time = millis_now;
        is_on = !is_on;  // toggle blinking
    }

    return is_on;
}

static void draw_timers_indicator(const state_machine_t timers[], uint8_t num_timers, uint8_t active_timer_index, bool active_timer_indicator_state)
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
            show_led = show_led && active_timer_indicator_state;
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
    draw_digit(top_digits[1], DIGITS_X_OFFSET + 7, TOP_Y_OFFSET);

    draw_digit(bottom_digits[0], DIGITS_X_OFFSET, BOTTOM_Y_OFFSET);
    draw_digit(bottom_digits[1], DIGITS_X_OFFSET + 7, BOTTOM_Y_OFFSET);

}

static bool is_paused_timer_blink_on()
{
    static bool is_on = true;
    uint16_t millis_now = millis();
    static uint16_t last_paused_timer_blink_time;

    if ((millis_now - last_paused_timer_blink_time) >= PAUSED_TIMER_BLINK_RATE) {
        last_paused_timer_blink_time = millis_now;
        is_on = !is_on;  // toggle blinking
    }

    return is_on;
}



void render_timer_view(state_machine_t* timers, uint8_t timer_count, uint8_t active_timer_index)
{
    state_machine_t* active_timer = &timers[active_timer_index];
    uint16_t current_time = active_timer->timer.current_time;
    state_t active_timer_state = active_timer->state;


    
    bool current_paused_timer_blink_is_on = is_paused_timer_blink_on();
    bool current_active_timer_indicator_blink_is_on = is_active_timer_indicator_blink_on();

    if (current_time == last_rendered_time && active_timer_state == last_rendered_state && current_active_timer_indicator_blink_is_on == last_active_timer_indicator_blink_is_on && current_paused_timer_blink_is_on == last_paused_timer_blink_is_on)
    {
        return;
    }

    matrix_buffer_clear();

    draw_timers_indicator(timers, timer_count, active_timer_index, current_active_timer_indicator_blink_is_on);
    if(active_timer_state != PAUSED || current_paused_timer_blink_is_on)
    {
        draw_active_timer(current_time);
    }

    matrix_update();

    last_rendered_time = current_time;
    last_rendered_state = active_timer_state;
    last_active_timer_indicator_blink_is_on = current_active_timer_indicator_blink_is_on;
    last_paused_timer_blink_is_on = current_paused_timer_blink_is_on;
}
