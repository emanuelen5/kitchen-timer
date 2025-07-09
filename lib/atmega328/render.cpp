#include "render.h"

#define DIGITS_X_OFFSET 2
#define DIGITS_Y_OFFSET 0
#define ACTIVE_TIMER_INDICATOR_BLINK_RATE 200
#define PAUSED_TIMER_BLINK_RATE 500
#define TIMERS_INDICATOR_COLUMN 0
#define FONT_WIDTH 6
#define FONT_HEIGHT 7

static uint16_t last_rendered_time;
static state_t last_rendered_state;
static bool last_paused_timer_blink_is_on;

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

static void blink_active_timer_indicator(uint8_t active_timer_index)
{
    static uint16_t last_blink_time;
    uint16_t millis_now = millis();
    static bool blink_state = true;

    if ( (millis_now - last_blink_time) >= ACTIVE_TIMER_INDICATOR_BLINK_RATE)
    {
        last_blink_time = millis_now;
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

static void draw_active_timer(uint16_t current_time, uint8_t x_offset, uint8_t y_offset)
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

    
    draw_digit(top_digits[0], x_offset, y_offset);
    draw_digit(top_digits[1], x_offset + 7, y_offset);

    draw_digit(bottom_digits[0], x_offset, y_offset + 8);
    draw_digit(bottom_digits[1], x_offset + 7, y_offset + 8);

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


void render_timer_view(state_machine_t* timers, uint8_t active_timer_index)
{
    state_machine_t* active_timer = &timers[active_timer_index];
    uint16_t current_time = active_timer->timer.current_time;
    state_t active_timer_state = active_timer->state;

    bool current_paused_timer_blink_is_on = is_paused_timer_blink_on();

    if (current_time == last_rendered_time && active_timer_state == last_rendered_state && current_paused_timer_blink_is_on == last_paused_timer_blink_is_on)
    {
        return;
    }

    matrix_buffer_clear();
    draw_timers_indicator(timers);
    blink_active_timer_indicator(active_timer_index);
    if(active_timer_state != PAUSED || current_paused_timer_blink_is_on)
    {
        draw_active_timer(current_time, DIGITS_X_OFFSET, DIGITS_Y_OFFSET);
    }
    matrix_update();

    last_rendered_time = current_time;
    last_rendered_state = active_timer_state;
    last_paused_timer_blink_is_on = current_paused_timer_blink_is_on;
}
