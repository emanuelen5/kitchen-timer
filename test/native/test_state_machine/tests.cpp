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

void set_counter(uint8_t count)
{
    (void)count;
}

void UART_printf(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
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
    TEST_ASSERT_EQUAL(get_state(&sm), IDLE);
    TEST_ASSERT_EQUAL(get_original_time(&sm), 0);
}

void test_when_in_idle_increment_timer_on_cw_rotation(void)
{
    state_machine_handle_event(&sm, CW_ROTATION);
    TEST_ASSERT_EQUAL(get_original_time(&sm), 1);
}

void test_when_in_idle_decrement_timer_on_ccw_rotation(void)
{
    sm.timer.original_time = 1;
    state_machine_handle_event(&sm, CCW_ROTATION);
    TEST_ASSERT_EQUAL(get_original_time(&sm), 0);
}

void test_when_in_idle_timer_doesnt_overflow(void)
{
    sm.timer.original_time = 0xffff;
    state_machine_handle_event(&sm, CW_ROTATION);
    TEST_ASSERT_EQUAL(get_original_time(&sm), 0xffff);
}

void test_when_in_idle_timer_doesnt_underflow(void)
{
    state_machine_handle_event(&sm, CCW_ROTATION);
    TEST_ASSERT_EQUAL(get_original_time(&sm), 0);
}

void test_when_running_it_counts_down_until_time_has_passed(void)
{
    set_state(&sm, RUNNING);
    sm.timer.original_time = 10;

    int actual_seconds = 0;
    while (true)
    {
        state_machine_handle_event(&sm, SECOND_TICK);
        actual_seconds++;
        if (get_state(&sm) != RUNNING)
            break;
    }
    TEST_ASSERT_EQUAL(10, actual_seconds);
    TEST_ASSERT_EQUAL(get_state(&sm), RINGING);
}

void test_ringing_exits_after_2000ms(void)
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

    TEST_ASSERT_EQUAL(2000, current_millis);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_initialize_as_idle);
    RUN_TEST(test_when_in_idle_increment_timer_on_cw_rotation);
    RUN_TEST(test_when_in_idle_decrement_timer_on_ccw_rotation);
    RUN_TEST(test_when_in_idle_timer_doesnt_overflow);
    RUN_TEST(test_when_in_idle_timer_doesnt_underflow);
    RUN_TEST(test_when_running_it_counts_down_until_time_has_passed);
    RUN_TEST(test_ringing_exits_after_2000ms);

    UNITY_END();
}
