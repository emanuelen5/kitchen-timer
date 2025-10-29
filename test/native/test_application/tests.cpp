#include <unity.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "application.h"
#include "helpers.h"
#include "interaction.h"

test_state_t test_state;

static void test_init(void)
{
    TEST_STATES_ARE("S...............");
}

static void test_add_new_timer(void)
{
    rotate(cw);
    double_press();

    TEST_STATES_ARE("sS..............");
}

static void test_move_between_timers(void)
{
    rotate(cw);
    double_press();
    rotate(cw);
    double_press();

    TEST_STATES_ARE("ssS.............");

    rotate_pressed(cw);
    TEST_STATES_ARE("ssS............");

    rotate_pressed(ccw);
    TEST_STATES_ARE("sSs............");

    rotate_pressed(ccw);
    TEST_STATES_ARE("Sss............");
}

static void test_timer_is_in_set_time_after_ringing(void)
{
    rotate(cw);
    press();
    TEST_STATES_ARE("R...............");
    tick_seconds(2);

    TEST_STATES_ARE("F...............");

    press();

    TEST_STATES_ARE("S...............");
}

static void test_timer_is_in_set_time_after_timeout(void)
{
    rotate(cw);
    press();
    tick_seconds(2);

    TEST_STATES_ARE("F...............");

    tick_seconds(10);

    TEST_STATES_ARE("S...............");
}

static void test_goes_to_menu_when_double_pressing_on_empty_timer(void)
{
    double_press();
    TEST_SHOWS_VIEW(MENU_VIEW);
}

static void test_timer_goes_to_idle_when_switching_from_it_when_it_is_empty(void)
{
    rotate(cw);
    double_press();
    TEST_STATES_ARE("sS..............");

    rotate_pressed(ccw);
    TEST_STATES_ARE("Si.............");
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_init);
    RUN_TEST(test_add_new_timer);
    RUN_TEST(test_move_between_timers);
    RUN_TEST(test_timer_is_in_set_time_after_ringing);
    RUN_TEST(test_timer_is_in_set_time_after_timeout);
    RUN_TEST(test_goes_to_menu_when_double_pressing_on_empty_timer);
    RUN_TEST(test_timer_goes_to_idle_when_switching_from_it_when_it_is_empty);

    UNITY_END();
}
