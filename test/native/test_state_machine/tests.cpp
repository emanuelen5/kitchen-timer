#include <unity.h>
#include <stdio.h>
#include <stdarg.h>

#include "state-machine.h"

state_machine_t sm;

void setUp(void)
{
    sm.init();
}

void tearDown(void)
{
}

void test_initialize_as_idle(void)
{
    TEST_ASSERT_TRUE(sm.is_idle());
}

void test_when_in_set_time_increment_timer_on_cw_rotation(void)
{
    sm.set_state(SET_TIME);
    sm.handle_event(CW_ROTATION);
    TEST_ASSERT_EQUAL(1, sm.get_target_time());
}

void test_when_in_set_time_decrement_timer_on_ccw_rotation(void)
{
    sm.set_state(SET_TIME);
    sm.timer.original_time = 1;
    sm.handle_event(CCW_ROTATION);
    TEST_ASSERT_EQUAL(0, sm.get_target_time());
}

void test_when_in_set_time_change_timer_more_on_fast_rotation(void)
{
    sm.set_state(SET_TIME);
    sm.handle_event(CW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(5, sm.get_target_time());
    sm.handle_event(CCW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(0, sm.get_target_time());
}

void test_when_in_set_time_and_above_an_hour_change_timer_in_minutes(void)
{
    sm.set_state(SET_TIME);
    sm.timer.original_time = 3600;
    sm.handle_event(CW_ROTATION);
    TEST_ASSERT_EQUAL(3660, sm.get_target_time());
    sm.handle_event(CCW_ROTATION);
    TEST_ASSERT_EQUAL(3600, sm.get_target_time());
}

void test_when_in_set_time_and_above_an_hour_change_timer_in_5_minutes_on_fast_rotation(void)
{
    sm.set_state(SET_TIME);
    sm.timer.original_time = 3600;
    sm.handle_event(CW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(3900, sm.get_target_time());
    sm.handle_event(CCW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(3600, sm.get_target_time());
}

void test_when_in_set_time_timer_doesnt_overflow(void)
{
    sm.set_state(SET_TIME);
    sm.timer.original_time = state_machine::max_time;
    sm.handle_event(CW_ROTATION);
    TEST_ASSERT_EQUAL(state_machine::max_time, sm.get_target_time());
}

void test_when_in_set_time_timer_doesnt_underflow(void)
{
    sm.set_state(SET_TIME);
    sm.handle_event(CCW_ROTATION);
    TEST_ASSERT_EQUAL(0, sm.get_target_time());
}

void test_when_running_it_counts_down_until_time_has_passed(void)
{
    sm.timer.original_time = 10;
    sm.set_state(RUNNING);

    int actual_seconds = 0;
    while (true)
    {
        sm.handle_event(SECOND_TICK);
        actual_seconds++;
        if (sm.get_state() != RUNNING)
            break;
    }
    TEST_ASSERT_EQUAL(actual_seconds, 10);
    TEST_ASSERT_EQUAL(RINGING, sm.get_state());
}

void test_when_running_rotation_changes_the_time_left(void)
{
    sm.timer.original_time = 10;
    sm.handle_event(SINGLE_PRESS);
    sm.handle_event(SECOND_TICK);
    TEST_ASSERT_EQUAL(9, sm.get_time_left());

    sm.handle_event(CW_ROTATION);
    TEST_ASSERT_EQUAL(10, sm.get_time_left());
    sm.handle_event(CCW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(5, sm.get_time_left());
}

void run_until_ringing(state_machine_t *sm, uint16_t seconds)
{
    sm->timer.original_time = seconds;
    sm->handle_event(SINGLE_PRESS);
    for (uint16_t i = 0; i < seconds; i++)
        sm->handle_event(SECOND_TICK);
    TEST_ASSERT_EQUAL(RINGING, sm->get_state());
}

void test_when_ringing_the_elapsed_time_keeps_counting_up(void)
{
    run_until_ringing(&sm, 3);
    TEST_ASSERT_EQUAL(3, sm.get_elapsed_time());

    sm.handle_event(SECOND_TICK);
    sm.handle_event(SECOND_TICK);
    TEST_ASSERT_EQUAL(5, sm.get_elapsed_time());
}

void test_elapsed_time_starts_over_the_next_time_it_runs(void)
{
    run_until_ringing(&sm, 3);
    sm.handle_event(SECOND_TICK);
    sm.handle_event(SINGLE_PRESS);

    run_until_ringing(&sm, 3);
    TEST_ASSERT_EQUAL(3, sm.get_elapsed_time());
}

void test_elapsed_time_doesnt_overflow(void)
{
    run_until_ringing(&sm, state_machine::max_time - 1);
    sm.handle_event(SECOND_TICK);
    sm.handle_event(SECOND_TICK);
    TEST_ASSERT_EQUAL(state_machine::max_time, sm.get_elapsed_time());
}

uint16_t run_until_state_times_out(state_machine_t *sm, state_t initial_state)
{
    uint16_t seconds = 0;
    while (true)
    {
        sm->handle_event(SECOND_TICK);
        seconds++;

        if (sm->get_state() != initial_state)
            return seconds;
        bool panic = seconds == 0;
        if (panic)
            TEST_FAIL_MESSAGE("The state was never left");
    }
}

void test_ringing_exits_after_5_minutes(void)
{
    sm.set_state(RINGING);

    uint16_t seconds = run_until_state_times_out(&sm, RINGING);

    TEST_ASSERT_EQUAL(5 * 60, seconds);
}

void test_ringing_timeout_counts_from_when_ringing_started(void)
{
    sm.timer.original_time = 10;
    sm.set_state(RUNNING);
    run_until_state_times_out(&sm, RUNNING);

    uint16_t seconds = run_until_state_times_out(&sm, RINGING);

    TEST_ASSERT_EQUAL(5 * 60, seconds);
}

void test_gh_issue_94_decrementing_below_zero_makes_it_wrap(void)
{
    sm.set_state(SET_TIME);
    sm.timer.original_time = 0;
    sm.handle_event(CCW_ROTATION);
    TEST_ASSERT_EQUAL(0, sm.get_target_time());
    sm.handle_event(CCW_ROTATION_FAST);
    TEST_ASSERT_EQUAL(0, sm.get_target_time());
}

void test_resets_target_time_when_timer_ends(void)
{
    sm.timer.original_time = 3;
    sm.set_state(RINGING);

    run_until_state_times_out(&sm, RINGING);
    TEST_ASSERT_EQUAL(0, sm.get_target_time());
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
    RUN_TEST(test_ringing_exits_after_5_minutes);
    RUN_TEST(test_ringing_timeout_counts_from_when_ringing_started);
    RUN_TEST(test_when_running_rotation_changes_the_time_left);
    RUN_TEST(test_when_ringing_the_elapsed_time_keeps_counting_up);
    RUN_TEST(test_elapsed_time_starts_over_the_next_time_it_runs);
    RUN_TEST(test_elapsed_time_doesnt_overflow);
    RUN_TEST(test_gh_issue_94_decrementing_below_zero_makes_it_wrap);
    RUN_TEST(test_resets_target_time_when_timer_ends);

    UNITY_END();
}
