#include <unity.h>
#include <stdio.h>
#include <string.h>

#include "application.h"
#include "helpers.h"

// Test doubles
uint16_t millis(void) { return test_state.current_millis; }
void enter_deep_sleep(void) { test_state.sleep_start_count++; }
void noToneAC() { test_state.melody_playing = false; }
void toneAC(unsigned long, uint8_t) { test_state.melody_playing = true; }
void max72xx_display_on(void) { test_state.is_display_on = true; }
void max72xx_display_off(void) { test_state.is_display_on = false; }
void max72xx_set_intensity(uint8_t brightness) { test_state.display_brightness = brightness; }

application_t app;

void setUp(void)
{
    test_state = {};
    test_state.display_brightness = 0xa;
    test_state.is_display_on = true;
    test_state.buzzer_volume = Buzzer::default_volume;
    init_application(&app);
}

void tearDown(void)
{
}

void tick_seconds(uint16_t seconds)
{
    for (uint16_t i = 0; i < seconds; i++)
    {
        for (uint16_t j = 0; j < 1000; j++)
        {
            test_state.current_millis++;
            service_application(&app);
        }
        application_handle_event(&app, SECOND_TICK);
        service_application(&app);
    }
}

// S = SET_TIME, I = IDLE, R = RUNNING, F = RINGING
// Capital letter if it's the selected timer
#define TEST_STATES_ARE(state_string) \
    assert_button_state(state_string, __LINE__)

void assert_button_state(const char *expected_state_str, uint32_t line)
{
    size_t len = strlen(expected_state_str);
    UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT16(MAX_TIMERS, len, line, "Too many timer states provided");
    for (size_t i = 0; i < len; i++)
    {
        char actual_states[len + 1];
        for (size_t j = 0; j < len; j++)
        {
            if (expected_state_str[j] == ' ' || expected_state_str[j] == '.')
            {
                actual_states[j] = expected_state_str[j];
                continue;
            }

            state_t actual_state = get_state(&app.state_machines[j]);
            bool is_active_sm = (j == app.current_active_sm);
            if (is_active_sm)
            {
                actual_states[j] =
                    (actual_state == SET_TIME)  ? 'S'
                    : (actual_state == IDLE)    ? 'I'
                    : (actual_state == RUNNING) ? 'R'
                    : (actual_state == PAUSED)  ? 'P'
                    : (actual_state == RINGING) ? 'F'
                                                : '?';
            }
            else
            {
                actual_states[j] =
                    (actual_state == SET_TIME)  ? 's'
                    : (actual_state == IDLE)    ? 'i'
                    : (actual_state == RUNNING) ? 'r'
                    : (actual_state == PAUSED)  ? 'p'
                    : (actual_state == RINGING) ? 'f'
                                                : '?';
            }
        }
        actual_states[len] = '\0';

        UNITY_TEST_ASSERT_EQUAL_STRING(expected_state_str, actual_states, line, "Timer states mismatch");
    }
}

void assert_shows_view(application_view_t view, uint32_t line)
{
    UNITY_TEST_ASSERT_EQUAL_STRING(application_view_to_string(view), application_view_to_string(app.current_view), line, "Expected to be in view");
}
