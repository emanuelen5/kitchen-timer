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
    TEST_ASSERT_TRUE(state_machine_is_idle(&sm));
}

void test_when_in_set_time_increment_timer_on_cw_rotation(void)
{
    set_state(&sm, SET_TIME);
    state_machine_handle_event(&sm, CW_ROTATION);
    TEST_ASSERT_EQUAL(1, get_target_time(&sm));
}

void test_when_in_set_time_decrement_timer_on_ccw_rotation(void)
{
    set_state(&sm, SET_TIME);
    sm.timer.original_time = 1;
    state_machine_handle_event(&sm, CCW_ROTATION);
    TEST_ASSERT_EQUAL(0, get_target_time(&sm));
}

void test_when_in_set_time_change_timer_more_on_fast_rotation(void)
{
    set_state(&sm, SET_TIME);
    state_machine_handle_event(&sm, CW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(5, get_target_time(&sm));
    state_machine_handle_event(&sm, CCW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(0, get_target_time(&sm));
}

void test_when_in_set_time_and_above_an_hour_change_timer_in_minutes(void)
{
    set_state(&sm, SET_TIME);
    sm.timer.original_time = 3600;
    state_machine_handle_event(&sm, CW_ROTATION);
    TEST_ASSERT_EQUAL(3660, get_target_time(&sm));
    state_machine_handle_event(&sm, CCW_ROTATION);
    TEST_ASSERT_EQUAL(3600, get_target_time(&sm));
}

void test_when_in_set_time_and_above_an_hour_change_timer_in_5_minutes_on_fast_rotation(void)
{
    set_state(&sm, SET_TIME);
    sm.timer.original_time = 3600;
    state_machine_handle_event(&sm, CW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(3900, get_target_time(&sm));
    state_machine_handle_event(&sm, CCW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(3600, get_target_time(&sm));
}

static void test_when_in_set_time_fast_cw_snaps_to_next_5_multiple_when_not_on_multiple(void)
{
    set_state(&sm, SET_TIME);
    sm.timer.original_time = 6;
    state_machine_handle_event(&sm, CW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(10, get_target_time(&sm));
}

static void test_when_in_set_time_fast_cw_snaps_to_5_from_below_5_multiple(void)
{
    set_state(&sm, SET_TIME);
    sm.timer.original_time = 3;
    state_machine_handle_event(&sm, CW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(5, get_target_time(&sm));
}

static void test_when_in_set_time_fast_cw_from_multiple_goes_to_next_multiple(void)
{
    set_state(&sm, SET_TIME);
    sm.timer.original_time = 5;
    state_machine_handle_event(&sm, CW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(10, get_target_time(&sm));
}

static void test_when_in_set_time_fast_ccw_snaps_to_prev_5_multiple_when_not_on_multiple(void)
{
    set_state(&sm, SET_TIME);
    sm.timer.original_time = 7;
    state_machine_handle_event(&sm, CCW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(5, get_target_time(&sm));
}

static void test_when_running_and_timer_has_ticked_fast_cw_uses_5_second_step(void)
{
    // Timer was set to 5, ticked down to 4; fast CW should add a full 5-second snap
    // (based on original_time=5, snapping gives new_original=10, new_current=9)
    sm.timer.original_time = 5;
    sm.timer.current_time = 4;
    set_state(&sm, RUNNING);
    state_machine_handle_event(&sm, CW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(10, get_target_time(&sm));
    TEST_ASSERT_EQUAL(9, get_time_left(&sm));
}

void test_when_in_set_time_timer_doesnt_overflow(void)
{
    set_state(&sm, SET_TIME);
    sm.timer.original_time = state_machine::max_time;
    state_machine_handle_event(&sm, CW_ROTATION);
    TEST_ASSERT_EQUAL(state_machine::max_time, get_target_time(&sm));
}

void test_when_in_set_time_timer_doesnt_underflow(void)
{
    set_state(&sm, SET_TIME);
    state_machine_handle_event(&sm, CCW_ROTATION);
    TEST_ASSERT_EQUAL(0, get_target_time(&sm));
}

void test_when_running_it_counts_down_until_time_has_passed(void)
{
    sm.timer.original_time = 10;
    set_current_time_to_target_time(&sm.timer);
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

void run_until_state_times_out(state_machine_t *sm, state_t initial_state)
{
    while (true)
    {
        current_millis++;
        service_state_machine(sm);

        if (get_state(sm) != initial_state)
            break;
        bool panic = current_millis == 0;
        if (panic)
            TEST_FAIL_MESSAGE("The state was never left");
    }
}

void test_ringing_exits_after_10000ms(void)
{
    set_state(&sm, RINGING);
    service_state_machine(&sm);

    run_until_state_times_out(&sm, RINGING);

    TEST_ASSERT_EQUAL(10000, current_millis);
}

void test_gh_issue_94_decrementing_below_zero_makes_it_wrap(void)
{
    set_state(&sm, SET_TIME);
    sm.timer.original_time = 0;
    state_machine_handle_event(&sm, CCW_ROTATION);
    TEST_ASSERT_EQUAL(0, get_target_time(&sm));
    state_machine_handle_event(&sm, CCW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(0, get_target_time(&sm));
}

void test_resets_target_time_when_timer_ends(void)
{
    sm.timer.original_time = 3;
    set_state(&sm, RINGING);

    run_until_state_times_out(&sm, RINGING);
    TEST_ASSERT_EQUAL(0, get_target_time(&sm));
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
    RUN_TEST(test_when_in_set_time_fast_cw_snaps_to_next_5_multiple_when_not_on_multiple);
    RUN_TEST(test_when_in_set_time_fast_cw_snaps_to_5_from_below_5_multiple);
    RUN_TEST(test_when_in_set_time_fast_cw_from_multiple_goes_to_next_multiple);
    RUN_TEST(test_when_in_set_time_fast_ccw_snaps_to_prev_5_multiple_when_not_on_multiple);
    RUN_TEST(test_when_running_and_timer_has_ticked_fast_cw_uses_5_second_step);
    RUN_TEST(test_when_in_set_time_timer_doesnt_overflow);
    RUN_TEST(test_when_in_set_time_timer_doesnt_underflow);
    RUN_TEST(test_when_running_it_counts_down_until_time_has_passed);
    RUN_TEST(test_ringing_exits_after_10000ms);
    RUN_TEST(test_gh_issue_94_decrementing_below_zero_makes_it_wrap);
    RUN_TEST(test_resets_target_time_when_timer_ends);

    UNITY_END();
}
