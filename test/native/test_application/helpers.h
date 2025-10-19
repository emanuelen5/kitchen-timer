#include <unity.h>
#include "application.h"

typedef struct
{
    int display_brightness;
    bool is_display_on;
    int buzzer_volume;
    bool melody_playing;
    int sleep_start_count;
    uint16_t current_millis = 0;
} test_state_t;

extern test_state_t test_state;

void tick_seconds(uint16_t seconds);

#define TEST_STATES_ARE(state_string) \
    assert_button_state(state_string, __LINE__)
void assert_button_state(const char *expected_state_str, uint32_t line);

#define TEST_SHOWS_VIEW(view) \
    assert_shows_view(view, __LINE__)
void assert_shows_view(application_view_t view, uint32_t line);
