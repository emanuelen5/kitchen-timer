#include <unity.h>
#include "button.h"

void on_single_press(void);
void on_double_press(void);
void on_long_press(void);

Button *btn;
class TestState
{
public:
    TestState()
        : time_ms(0), single_presses(0), double_presses(0), long_presses(0){};

    uint16_t time_ms;
    int single_presses, double_presses, long_presses;

    void increment_time(uint16_t ms)
    {
        set_time(time_ms + ms);
    }

    void set_time(uint16_t ms)
    {
        TEST_ASSERT_TRUE_MESSAGE(ms > time_ms, "Cannot go back in time");
        while (time_ms < ms)
        {
            time_ms++;
            // We have to call the service routine every millisecond to
            // guarantee operation
            btn->service();
        }
    }
};

TestState *state;

void setUp(void)
{
    state = new TestState();
    btn = new Button(&on_single_press, &on_double_press, &on_long_press);
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

void on_single_press()
{
    state->single_presses++;
}

void on_double_press()
{
    state->double_presses++;
}

void on_long_press()
{
    state->long_presses++;
}

#define TEST_ASSERT_BUTTON_STATE(singles, doubles, longs) \
    assert_button_state(singles, doubles, longs, __LINE__)

void assert_button_state(int single_press, int double_press, int long_press, uint32_t line)
{
    UNITY_TEST_ASSERT_EQUAL_INT(single_press, state->single_presses, line, "Single press count");
    UNITY_TEST_ASSERT_EQUAL_INT(double_press, state->double_presses, line, "Double press count");
    UNITY_TEST_ASSERT_EQUAL_INT(long_press, state->long_presses, line, "Long press count");
}

void test_single_press(void)
{
    btn->press();
    state->increment_time(1);
    btn->release();

    state->set_time(Button::double_press_timeout_ms);
    TEST_ASSERT_BUTTON_STATE(false, false, false);

    state->set_time(Button::double_press_timeout_ms + 1);
    TEST_ASSERT_BUTTON_STATE(1, false, false);
}

void test_double_press(void)
{
    btn->press();
    state->set_time(10);
    btn->release();
    state->set_time(20);
    TEST_ASSERT_BUTTON_STATE(false, false, false);

    btn->press();
    TEST_ASSERT_BUTTON_STATE(false, true, false);
}

void test_long_press(void)
{
    btn->press();
    state->increment_time(Button::long_press_threshold_ms + 1);
    TEST_ASSERT_BUTTON_STATE(false, false, false);

    btn->release();

    TEST_ASSERT_BUTTON_STATE(false, false, 1);
}

void test_single_press_too_slow_for_double(void)
{
    btn->press();
    state->set_time(Button::double_press_timeout_ms - 1);
    btn->release();

    state->set_time(Button::double_press_timeout_ms + 1);
    TEST_ASSERT_BUTTON_STATE(1, false, false);
}

void test_press_twice(void)
{
    btn->press();
    state->increment_time(Button::double_press_timeout_ms / 2 + 1);
    btn->release();
    state->increment_time(Button::double_press_timeout_ms / 2 + 1);

    TEST_ASSERT_BUTTON_STATE(1, false, false);

    btn->press();
    state->increment_time(Button::double_press_timeout_ms / 2 + 1);
    btn->release();
    state->increment_time(Button::double_press_timeout_ms / 2 + 1);

    TEST_ASSERT_BUTTON_STATE(2, false, false);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_single_press);
    RUN_TEST(test_double_press);
    RUN_TEST(test_long_press);
    RUN_TEST(test_single_press_too_slow_for_double);
    RUN_TEST(test_press_twice);

    UNITY_END();

    return 0;
}
