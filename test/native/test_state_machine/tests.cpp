#include <unity.h>
#include <stdio.h>
#include <stdarg.h>

#include "state-machine.h"

// Test doubles
uint16_t current_millis = 0;
uint16_t millis(void)
{
    return current_millis;
}

state_machine_t sm;

void setUp(void)
{
    current_millis = 0;
    init_state_machine(&sm);
}

void tearDown(void)
{
}

void test_initialize_as_idle(void)
{
    TEST_ASSERT_EQUAL(IDLE, get_state(&sm));
    TEST_ASSERT_EQUAL(0, get_original_time(&sm));
}

void test_when_in_set_time_increment_timer_on_cw_rotation(void)
{
    set_state(&sm, SET_TIME);
    state_machine_handle_event(&sm, CW_ROTATION);
    TEST_ASSERT_EQUAL(1, get_original_time(&sm));
}

void test_when_in_set_time_decrement_timer_on_ccw_rotation(void)
{
    set_state(&sm, SET_TIME);
    sm.timer.original_time = 1;
    state_machine_handle_event(&sm, CCW_ROTATION);
    TEST_ASSERT_EQUAL(0, get_original_time(&sm));
}

void test_when_in_set_time_change_timer_more_on_fast_rotation(void)
{
    set_state(&sm, SET_TIME);
    state_machine_handle_event(&sm, CW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(5, get_original_time(&sm));
    state_machine_handle_event(&sm, CCW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(0, get_original_time(&sm));
}

void test_when_in_set_time_and_above_an_hour_change_timer_in_minutes(void)
{
    set_state(&sm, SET_TIME);
    sm.timer.original_time = 3600;
    state_machine_handle_event(&sm, CW_ROTATION);
    TEST_ASSERT_EQUAL(3660, get_original_time(&sm));
    state_machine_handle_event(&sm, CCW_ROTATION);
    TEST_ASSERT_EQUAL(3600, get_original_time(&sm));
}

void test_when_in_set_time_and_above_an_hour_change_timer_in_5_minutes_on_fast_rotation(void)
{
    set_state(&sm, SET_TIME);
    sm.timer.original_time = 3600;
    state_machine_handle_event(&sm, CW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(3900, get_original_time(&sm));
    state_machine_handle_event(&sm, CCW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(3600, get_original_time(&sm));
}

void test_when_in_set_time_timer_doesnt_overflow(void)
{
    set_state(&sm,SET_TIME);
    sm.timer.original_time = 0xffff;
    state_machine_handle_event(&sm, CW_ROTATION);
    TEST_ASSERT_EQUAL(0xffff, get_original_time(&sm));
}

void test_when_in_set_time_timer_doesnt_underflow(void)
{
    set_state(&sm, SET_TIME);
    state_machine_handle_event(&sm, CCW_ROTATION);
    TEST_ASSERT_EQUAL(0, get_original_time(&sm));
}

void test_when_running_it_counts_down_until_time_has_passed(void)
{
    sm.timer.original_time = 10;
    copy_original_to_current_time(&sm.timer);
    set_state(&sm, RUNNING);

    int actual_seconds = 0;
    while (true)
    {
        state_machine_handle_event(&sm, SECOND_TICK);
        actual_seconds++;
        if (get_state(&sm) != RUNNING)
            break;
    }
    TEST_ASSERT_EQUAL(actual_seconds, 10);
    TEST_ASSERT_EQUAL(RINGING, get_state(&sm));
}

void test_ringing_exits_after_10000ms(void)
{
    set_state(&sm, RINGING);
    service_state_machine(&sm);

    current_millis = 0;
    while (true)
    {
        current_millis++;
        service_state_machine(&sm);

        if (get_state(&sm) != RINGING)
            break;
        bool panic = current_millis == 0;
        if (panic)
            TEST_FAIL_MESSAGE("The state RINGING was never left");
    }

    TEST_ASSERT_EQUAL(current_millis, 10000);
}

void test_gh_issue_94_decrementing_below_zero_makes_it_wrap(void)
{
    set_state(&sm, SET_TIME);
    sm.timer.original_time = 0;
    state_machine_handle_event(&sm, CCW_ROTATION);
    TEST_ASSERT_EQUAL(0, get_original_time(&sm));
    state_machine_handle_event(&sm, CCW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(0, get_original_time(&sm));
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_initialize_as_idle);
    RUN_TEST(test_when_in_set_time_increment_timer_on_cw_rotation);
    RUN_TEST(test_when_in_set_time_decrement_timer_on_ccw_rotation);
    RUN_TEST(test_when_in_set_time_change_timer_more_on_fast_rotation);
    RUN_TEST(test_when_in_set_time_and_above_an_hour_change_timer_in_minutes);
    RUN_TEST(test_when_in_set_time_and_above_an_hour_change_timer_in_5_minutes_on_fast_rotation);
    RUN_TEST(test_when_in_set_time_timer_doesnt_overflow);
    RUN_TEST(test_when_in_set_time_timer_doesnt_underflow);
    RUN_TEST(test_when_running_it_counts_down_until_time_has_passed);
    RUN_TEST(test_ringing_exits_after_10000ms);
    RUN_TEST(test_gh_issue_94_decrementing_below_zero_makes_it_wrap);

    UNITY_END();
}
