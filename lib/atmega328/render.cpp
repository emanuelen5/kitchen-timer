#include <stdint.h>
#include "render.h"
#include "util.h"
#include "config.h"
#include "fat_font.h"
#include "max72xx_matrix.h"
#include "max72xx.h"
#include "millis.h"
#include "UART.h"
#include <avr/pgmspace.h>
#include "battery_measure.h"

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

static void draw_timer_indicators(state_machine_t sm[])
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

static void draw_char(char c, uint8_t x_offset, uint8_t y_offset, bool clear_digit)
{
    const uint8_t *bitmap = get_bitmap(c);

    for (uint8_t row = 0; row < FATFONT_HEIGHT; row++)
    {
        for (uint8_t col = 0; col < FATFONT_WIDTH; col++)
        {
            bool is_on = false;
            if (!clear_digit)
            {
                is_on = bitmap[row] & bit(FATFONT_WIDTH - 1 - col); // 6-bit wide
            }
            matrix_set_pixel(x_offset + col, y_offset + row, is_on);
        }
    }
}

static void draw_bitmap(const uint8_t *bitmap, uint8_t width, uint8_t height,
                        uint8_t bytes_per_row, uint8_t x_offset,
                        uint8_t y_offset, bool clear_bitmap)
{
    for (uint8_t row = 0; row < height; row++)
    {
        for (uint8_t col = 0; col < width; col++)
        {
            bool is_on = false;
            if (!clear_bitmap)
            {
                uint16_t byte_index = col / 8;
                if (byte_index < bytes_per_row)
                {
                    uint8_t bit_in_byte = col % 8; // 0..7 with LSB=0
                    uint8_t bit_mask = (1 << (7 - bit_in_byte));

                    uint8_t raw_byte = pgm_read_byte(bitmap + (row * bytes_per_row + byte_index));
                    is_on = (raw_byte & bit_mask) != 0;
                }
                else
                {
                    is_on = false;
                }
            }
            matrix_set_pixel(x_offset + col, y_offset + row, is_on);
        }
    }
}

static void draw_active_timer(uint16_t current_time, uint8_t x_offset, uint8_t y_offset, bool clear_active_timer)
{
    const bool show_as_hh_mm = current_time >= 3600;
    uint8_t top_value, bottom_value;
    if (show_as_hh_mm)
    {
        top_value = current_time / 3600;
        bottom_value = (current_time / 60) % 60;
    }
    else
    { // mm_ss
        top_value = current_time / 60;
        bottom_value = current_time % 60;
    }

    char top_digits[2], bottom_digits[2];
    if (show_as_hh_mm)
    {
        top_digits[0] = '0' + (top_value % 10);
        top_digits[1] = 'h';
    }
    else
    {
        top_digits[0] = '0' + (top_value / 10);
        top_digits[1] = '0' + (top_value % 10);
    }

    bottom_digits[0] = '0' + (bottom_value / 10);
    bottom_digits[1] = '0' + (bottom_value % 10);

    draw_char(top_digits[0], x_offset, y_offset, clear_active_timer);
    draw_char(top_digits[1], x_offset + 7, y_offset, clear_active_timer);

    draw_char(bottom_digits[0], x_offset, y_offset + 8, clear_active_timer);
    draw_char(bottom_digits[1], x_offset + 7, y_offset + 8, clear_active_timer);
}

static void draw_voltage(uint16_t centivolts)
{
    const uint8_t x_offset = 0, y_offset = 0;
    uint8_t digits[3];
    digits[0] = '0' + (centivolts / 100);        // Volts
    digits[1] = '0' + ((centivolts % 100) / 10); // First decimal
    digits[2] = '0' + (centivolts % 10);         // Second decimal

    draw_char('V', x_offset, y_offset, false);
    draw_char(digits[0], x_offset + 7, y_offset, false);
    matrix_set_pixel(x_offset + 14, y_offset + 6, true); // Decimal point

    draw_char(digits[1], x_offset, y_offset + 8, false);
    draw_char(digits[2], x_offset + 7, y_offset + 8, false);
}

static blink_state_t timer_digits_blink = {0, true};
void render_active_timer_view(state_machine_t *state_machines, uint8_t active_timer_index)
{
    uint16_t time_to_display;
    state_machine_t *active_sm = &state_machines[active_timer_index];
    bool blink = get_blink_state(&timer_digits_blink, TIMER_DIGITS_BLINK_RATE);

    switch (active_sm->state)
    {
    case SET_TIME:
        time_to_display = active_sm->timer.original_time;
        break;

    case RUNNING:
        time_to_display = active_sm->timer.current_time;
        break;

    case PAUSED:
        time_to_display = active_sm->timer.current_time;
        break;

    case RINGING:
        time_to_display = get_target_time(&active_sm->timer);
        break;
    default:
        // Do nothing
        break;
    }

    draw_timer_indicators(state_machines);
    draw_ringing_indicator(state_machines);
    bool should_blink_timer_numbers = active_sm->state == PAUSED || active_sm->state == RINGING;
    if (should_blink_timer_numbers)
    {
        if (blink)
        {
            draw_active_timer(time_to_display, DIGITS_X_OFFSET, DIGITS_Y_OFFSET, false);
        }
    }
    else
    {
        draw_active_timer(time_to_display, DIGITS_X_OFFSET, DIGITS_Y_OFFSET, false);
    }
    draw_active_timer_indicator(active_timer_index);
}

static void render_settings_menu_view(application_t *app)
{
    switch (app->settings_menu.current_menu_position)
    {
    case BRIGHTNESS:
        draw_bitmap(get_icon_bitmap(icon_brightness), MATRIX_COL_WIDTH, MATRIX_ROW_HEIGHT, 2, 0, 1, false);
        break;

    case VOLUME:
        draw_bitmap(get_icon_bitmap(icon_volume), MATRIX_COL_WIDTH, MATRIX_ROW_HEIGHT, 2, 0, 0, false);
        break;

    case BATTERY_V:
        draw_bitmap(get_icon_bitmap(icon_battery), MATRIX_COL_WIDTH, MATRIX_ROW_HEIGHT, 2, 0, 0, false);
        break;

    case MELODY:
        draw_bitmap(get_icon_bitmap(icon_melody), MATRIX_COL_WIDTH, MATRIX_ROW_HEIGHT, 2, 0, 0, false);
        break;

    case SNAKE:
        draw_bitmap(get_icon_bitmap(icon_snake), MATRIX_COL_WIDTH, MATRIX_ROW_HEIGHT, 2, 0, 0, false);
        break;

    case BACK:
        draw_bitmap(get_icon_bitmap(icon_return), MATRIX_COL_WIDTH, MATRIX_ROW_HEIGHT, 2, 0, 0, false);
        break;

    default:
        break;
    }
}

static void render_brightness_setting_view(application_t *app)
{
    draw_bitmap(get_icon_bitmap(icon_brightness), MATRIX_COL_WIDTH, MATRIX_ROW_HEIGHT, 2, 0, 1, false);
    for (int i = 0; i <= max72xx_max_brightness; i++)
    {
        const bool is_on = (app->brightness) >= i;
        matrix_set_pixel(i, 0, is_on);
    }
}

static void render_volume_setting_view(application_t *app)
{
    uint8_t volume = app->buzzer.get_volume();
    draw_bitmap(get_icon_bitmap(icon_volume), MATRIX_COL_WIDTH, MATRIX_ROW_HEIGHT, 2, 0, 0, false);
    for (int i = 0; i <= app->buzzer.max_volume; i++)
    {
        const bool is_on = volume > i;
        matrix_set_pixel(i, 0, is_on);
    }
}

static void render_battery_setting_view(application_t *app)
{
    draw_voltage(get_average_battery_voltage(&app->battery_measurement));
    for (uint8_t i = 0; i < 16; i++)
    {
        bool is_on = i < app->battery_measurement.measurements_taken && !battery_measurement_is_complete(&app->battery_measurement);
        matrix_set_pixel(i, 15, is_on);
    }
}

static void render_melody_setting_view(application_t *app)
{
    const uint8_t selected_melody_number = (uint8_t)app->selected_melody + 1;
    const char melody_number_char = (char)('0' + selected_melody_number);

    draw_char('M', 0, 4, false);
    draw_char(melody_number_char, 7, 4, false);
}

static void render_snake_view(application_t *app)
{
    snake_game_t *game = &app->snake_game;

    for (uint16_t i = 1; i < game->length; i++)
    {
        matrix_set_pixel(game->body[i].x, game->body[i].y, true);
    }

    const bool show_food_and_head = game->status != SNAKE_PAUSED || get_blink_state(&timer_digits_blink, TIMER_DIGITS_BLINK_RATE);
    matrix_set_pixel(game->body[0].x, game->body[0].y, show_food_and_head);

    
    matrix_set_pixel(game->food.x, game->food.y, show_food_and_head);

    if (game->status == SNAKE_GAME_OVER)
    {
        for (uint8_t i = 0; i < MATRIX_COL_WIDTH; i++)
        {
            matrix_set_pixel(i, i, true);
            matrix_set_pixel(MATRIX_COL_WIDTH - 1 - i, i, true);
        }
    }
    else if (game->status == SNAKE_WON)
    {
        for (uint8_t i = 0; i < MATRIX_COL_WIDTH; i++)
        {
            matrix_set_pixel(i, 0, true);
            matrix_set_pixel(i, MATRIX_ROW_HEIGHT - 1, true);
            matrix_set_pixel(0, i, true);
            matrix_set_pixel(MATRIX_COL_WIDTH - 1, i, true);
        }
    }
}

void render(application_t *app)
{
    matrix_buffer_clear();
    switch (app->current_view)
    {
    case ACTIVE_TIMER_VIEW:
        render_active_timer_view(app->state_machines, app->current_active_sm);
        break;

    case SETTINGS_MENU_VIEW:
        render_settings_menu_view(app);
        break;

    case BRIGHTNESS_SETTING_VIEW:
        render_brightness_setting_view(app);
        break;

    case VOLUME_SETTING_VIEW:
        render_volume_setting_view(app);
        break;

    case BATTERY_CHARGE_VIEW:
    {
        render_battery_setting_view(app);
        break;
    }

    case MELODY_SELECT_VIEW:
        render_melody_setting_view(app);
        break;

    case SNAKE_VIEW:
        render_snake_view(app);
        break;

    default:
        break;
    }
    matrix_update();
}
