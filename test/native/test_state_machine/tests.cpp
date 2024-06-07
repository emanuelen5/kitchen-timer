#include <unity.h>
#include <stdio.h>
#include <stdarg.h>

#include "state-machine.h"

// Test doubles
uint16_t millis(void)
{
    return 0;
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
    step_state(&sm, CW_ROTATION);
    TEST_ASSERT_EQUAL(get_original_time(&sm), 1);
}

void test_when_in_idle_decrement_timer_on_ccw_rotation(void)
{
    sm.timer.original_time = 1;
    step_state(&sm, CCW_ROTATION);
    TEST_ASSERT_EQUAL(get_original_time(&sm), 0);
}

void test_when_in_idle_timer_doesnt_overflow(void)
{
    sm.timer.original_time = 0xffff;
    step_state(&sm, CW_ROTATION);
    TEST_ASSERT_EQUAL(get_original_time(&sm), 0xffff);
}

void test_when_in_idle_timer_doesnt_underflow(void)
{
    step_state(&sm, CCW_ROTATION);
    TEST_ASSERT_EQUAL(get_original_time(&sm), 0);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_initialize_as_idle);
    RUN_TEST(test_when_in_idle_increment_timer_on_cw_rotation);
    RUN_TEST(test_when_in_idle_decrement_timer_on_ccw_rotation);
    RUN_TEST(test_when_in_idle_timer_doesnt_overflow);
    RUN_TEST(test_when_in_idle_timer_doesnt_underflow);

    UNITY_END();
}