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

class TestTimer : public Timer
{
public:
    uint16_t original_time;
};

class TestStateMachine : public KitchenTimerStateMachine
{
public:
    state_t state;
    TestTimer timer;
};

TestStateMachine sm;

void setUp(void)
{
    current_millis = 0;
    sm.reset();
}

void tearDown(void)
{
}

void test_initialize_as_idle(void)
{
    TEST_ASSERT_EQUAL(sm.get_state(), IDLE);
    TEST_ASSERT_EQUAL(sm.get_target_time(), 0);
}

void test_when_in_idle_increment_timer_on_cw_rotation(void)
{
    sm.handle_event(CW_ROTATION);
    TEST_ASSERT_EQUAL(sm.get_target_time(), 1);
}

void test_when_in_idle_decrement_timer_on_ccw_rotation(void)
{
    sm.timer.original_time = 1;
    sm.handle_event(CCW_ROTATION);
    TEST_ASSERT_EQUAL(sm.get_target_time(), 0);
}

void test_when_in_idle_timer_doesnt_overflow(void)
{
    sm.timer.original_time = 0xffff;
    sm.handle_event(CW_ROTATION);
    TEST_ASSERT_EQUAL(sm.get_target_time(), 0xffff);
}

void test_when_in_idle_timer_doesnt_underflow(void)
{
    sm.handle_event(CCW_ROTATION);
    TEST_ASSERT_EQUAL(sm.get_target_time(), 0);
}

void test_when_running_it_counts_down_until_time_has_passed(void)
{
    sm.state = RUNNING;
    sm.timer.original_time = 10;

    int actual_seconds = 0;
    while (true)
    {
        sm.handle_event(SECOND_TICK);
        actual_seconds++;
        if (sm.get_state() != RUNNING)
            break;
    }
    TEST_ASSERT_EQUAL(10, actual_seconds);
    TEST_ASSERT_EQUAL(sm.get_state(), RINGING);
}

void test_ringing_exits_after_2000ms(void)
{
    sm.state = RINGING;
    sm.service();

    current_millis = 0;
    while (true)
    {
        current_millis++;
        sm.service();

        if (sm.get_state() != RINGING)
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