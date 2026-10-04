#include <unity.h>

#include "application.h"
#include "settings.h"

// Test doubles
uint16_t current_millis = 0;
uint16_t millis(void)
{
    return current_millis;
}

bool display_is_on = true;
void max72xx_display_on(void) { display_is_on = true; }
void max72xx_display_off(void) { display_is_on = false; }
void max72xx_set_intensity(uint8_t) {}
void enter_deep_sleep(void) {}
void toneAC(unsigned long, uint8_t) {}
void noToneAC() {}
void save_byte_setting(uint8_t, eeprom_address) {}
void load_byte_setting(uint8_t *setting, eeprom_address) { *setting = 0; }
void minimize_battery_voltage_jitter(void) {}
uint16_t battery_centivolts(void) { return 0; }

application_t app;

void setUp(void)
{
    current_millis = 0;
    display_is_on = true;
    init_application(&app);
}

void tearDown(void)
{
}

static state_machine_t *active_sm(void)
{
    return &app.state_machines[app.current_active_sm];
}

static void fall_asleep(void)
{
    for (uint16_t i = 0; i < 1000 && !app.power_save.is_asleep(); i++)
        application_handle_event(&app, SECOND_TICK);
    TEST_ASSERT_TRUE(app.power_save.is_asleep());
    TEST_ASSERT_FALSE(display_is_on);
}

void test_when_asleep_single_press_only_wakes_it_up(void)
{
    active_sm()->timer.original_time = 10;
    fall_asleep();
    application_handle_event(&app, SINGLE_PRESS);
    TEST_ASSERT_TRUE(display_is_on);
    TEST_ASSERT_FALSE(app.power_save.is_asleep());
    TEST_ASSERT_EQUAL(SET_TIME, active_sm()->get_state());
}

void test_after_waking_up_single_press_starts_the_timer(void)
{
    active_sm()->timer.original_time = 10;
    fall_asleep();
    application_handle_event(&app, SINGLE_PRESS);
    application_handle_event(&app, SINGLE_PRESS);
    TEST_ASSERT_EQUAL(RUNNING, active_sm()->get_state());
}

void test_when_asleep_rotation_only_wakes_it_up(void)
{
    fall_asleep();
    application_handle_event(&app, CW_ROTATION);
    TEST_ASSERT_EQUAL(0, active_sm()->get_target_time());
    application_handle_event(&app, CW_ROTATION);
    TEST_ASSERT_EQUAL(1, active_sm()->get_target_time());
}

void test_when_asleep_double_press_only_wakes_it_up(void)
{
    fall_asleep();
    application_handle_event(&app, DOUBLE_PRESS);
    TEST_ASSERT_EQUAL(ACTIVE_TIMER_VIEW, app.current_view);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_when_asleep_single_press_only_wakes_it_up);
    RUN_TEST(test_after_waking_up_single_press_starts_the_timer);
    RUN_TEST(test_when_asleep_rotation_only_wakes_it_up);
    RUN_TEST(test_when_asleep_double_press_only_wakes_it_up);

    UNITY_END();
}
