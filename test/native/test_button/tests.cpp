#include <unity.h>
#include "button.h"

Button *btn;
class TestState
{
public:
    TestState()
        : time_ms(0), single_presss_detected(false), double_press_detected(false), long_press_detected(false){};

    uint16_t time_ms;
    bool single_presss_detected, double_press_detected, long_press_detected;

    void increment_time(uint16_t ms)
    {
        time_ms += ms;
        btn->service();
    }
};

TestState *state;

void setUp(void)
{
    state = new TestState();
    btn = new Button();
}

uint16_t millis(void)
{
    return state->time_ms;
}

void tearDown(void)
{
    delete state;
    delete btn;
}

void Button::on_single_press()
{
    state->single_presss_detected = true;
}

void Button::on_double_press()
{
    state->double_press_detected = true;
}

void Button::on_long_press()
{
    state->long_press_detected = true;
}

void test_single_press(void)
{
    btn->press();
    state->increment_time(1);
    btn->release();
    state->increment_time(Button::double_press_timeout_ms);

    TEST_ASSERT_TRUE(state->single_presss_detected);
}

void test_double_press(void)
{
    btn->press();
    state->increment_time(10);
    btn->release();
    state->increment_time(10);
    TEST_ASSERT_FALSE(state->double_press_detected);

    btn->press();
    TEST_ASSERT_TRUE(state->double_press_detected);

    state->increment_time(Button::double_press_timeout_ms);
    TEST_ASSERT_TRUE(state->double_press_detected);
}

void test_long_press(void)
{
    btn->press();
    state->increment_time(Button::long_press_threshold_ms + 1);
    btn->release();

    TEST_ASSERT_TRUE(state->long_press_detected);
}

void test_single_press_too_slow_for_double(void)
{
    btn->press();
    state->increment_time(Button::double_press_timeout_ms - 1);
    btn->release();

    state->increment_time(500); // Wait 60ms before the next press (too slow for double press)

    btn->press();
    state->increment_time(30);
    btn->release();

    TEST_ASSERT_TRUE(state->single_presss_detected);
    TEST_ASSERT_FALSE(state->double_press_detected);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_single_press);
    RUN_TEST(test_double_press);
    RUN_TEST(test_long_press);
    RUN_TEST(test_single_press_too_slow_for_double);

    UNITY_END();

    return 0;
}
