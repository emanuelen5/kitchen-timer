#include <unity.h>
#include <vector>
#include <string>
#include "button.h"
#include "events.h"

void on_single_press(void);
void on_double_press(void);
void on_long_press(void);
void on_rotation(rotation_dir_t dir, rotation_speed_t speed, bool held_down);

Button *btn;
class TestState
{
public:
    TestState()
        : time_ms(0) {};

    uint16_t time_ms;
    std::vector<event_t> events;

    void increment_time(uint16_t ms, bool should_service = true)
    {
        set_time(time_ms + ms, should_service);
    }

    void set_time(uint16_t ms, bool should_service = true)
    {
        TEST_ASSERT_TRUE_MESSAGE(ms > time_ms, "Cannot go back in time");
        while (time_ms < ms)
        {
            time_ms++;
            // We have to call the service routine every millisecond to
            // guarantee operation
            if (should_service)
                btn->service();
        }
    }
};

TestState *state;

void setUp(void)
{
    state = new TestState();
    btn = new Button(&on_single_press, &on_double_press, &on_long_press, &on_rotation);
}

uint16_t millis(void)
{
    return state->time_ms;
}

void tearDown(void)
{
    delete state;
}

void on_single_press()
{
    state->events.push_back(SINGLE_PRESS);
}

void on_double_press()
{
    state->events.push_back(DOUBLE_PRESS);
}

void on_long_press()
{
    state->events.push_back(LONG_PRESS);
}

void on_rotation(rotation_dir_t dir, rotation_speed_t speed, bool held_down)
{
    if (held_down)
        state->events.push_back(dir == cw ? CW_PRESSED_ROTATION : CCW_PRESSED_ROTATION);
    else if (speed == fast)
        state->events.push_back(dir == cw ? CW_ROTATION_FAST : CCW_ROTATION_FAST);
    else
        state->events.push_back(dir == cw ? CW_ROTATION : CCW_ROTATION);
}

static const char *event_to_string(event_t event)
{
    switch (event)
    {
    case SINGLE_PRESS:         return "SINGLE_PRESS";
    case DOUBLE_PRESS:         return "DOUBLE_PRESS";
    case LONG_PRESS:           return "LONG_PRESS";
    case CW_ROTATION:          return "CW_ROTATION";
    case CCW_ROTATION:         return "CCW_ROTATION";
    case CW_ROTATION_FAST:     return "CW_ROTATION_FAST";
    case CCW_ROTATION_FAST:    return "CCW_ROTATION_FAST";
    case CW_PRESSED_ROTATION:  return "CW_PRESSED_ROTATION";
    case CCW_PRESSED_ROTATION: return "CCW_PRESSED_ROTATION";
    default:                   return "UNKNOWN_EVENT";
    }
}

static std::string events_to_string(const std::vector<event_t> &events)
{
    std::string s = "[";
    for (size_t i = 0; i < events.size(); i++)
    {
        if (i != 0)
            s += ", ";
        s += event_to_string(events[i]);
    }
    return s + "]";
}

// Asserts that exactly the given events have been invoked, in the given order
#define TEST_ASSERT_EVENTS(...) \
    assert_events({__VA_ARGS__}, __LINE__)

void assert_events(const std::vector<event_t> &expected, uint32_t line)
{
    UNITY_TEST_ASSERT_EQUAL_STRING(events_to_string(expected).c_str(),
                                   events_to_string(state->events).c_str(),
                                   line, "Invoked events");
}

void test_single_press(void)
{
    btn->press();
    state->increment_time(1);
    btn->release();

    state->set_time(Button::double_press_timeout_ms);
    TEST_ASSERT_EVENTS();

    state->set_time(Button::double_press_timeout_ms + 1);
    TEST_ASSERT_EVENTS(SINGLE_PRESS);
}

void test_double_press(void)
{
    btn->press();
    state->set_time(10);
    btn->release();
    state->set_time(20);
    TEST_ASSERT_EVENTS();

    btn->press();
    TEST_ASSERT_EVENTS(DOUBLE_PRESS);
}

void test_long_press_is_registered_on_release(void)
{
    bool should_service = false;
    btn->press();
    state->increment_time(Button::long_press_threshold_ms + 1, should_service);
    TEST_ASSERT_EVENTS();

    btn->release();

    TEST_ASSERT_EVENTS(LONG_PRESS);
}

void test_long_press_isnt_triggered_when_rotating(void)
{
    btn->press();
    btn->rotate(cw, slow);
    state->increment_time(Button::long_press_threshold_ms + 1);
    btn->release();

    TEST_ASSERT_EVENTS(CW_PRESSED_ROTATION);
}

void test_press_is_triggered_before_rotation_when_pressing_and_then_rotating(void)
{
    btn->press();
    state->increment_time(Button::double_press_timeout_ms - 1);
    btn->release();
    TEST_ASSERT_EVENTS();

    btn->rotate(cw, slow);
    TEST_ASSERT_EVENTS(SINGLE_PRESS, CW_ROTATION);
    state->set_time(Button::long_press_threshold_ms + 1);
    TEST_ASSERT_EVENTS(SINGLE_PRESS, CW_ROTATION);
}

void test_press_isnt_triggered_when_holding_down_and_rotating(void)
{
    btn->press();
    state->increment_time(Button::double_press_timeout_ms - 1);
    TEST_ASSERT_EVENTS();

    btn->rotate(ccw, slow);
    btn->release();
    TEST_ASSERT_EVENTS(CCW_PRESSED_ROTATION);
    state->set_time(Button::long_press_threshold_ms + 1);
    TEST_ASSERT_EVENTS(CCW_PRESSED_ROTATION);
}

void test_rotation_speed_is_forwarded(void)
{
    btn->rotate(cw, fast);
    btn->rotate(ccw, fast);
    btn->rotate(ccw, slow);

    TEST_ASSERT_EVENTS(CW_ROTATION_FAST, CCW_ROTATION_FAST, CCW_ROTATION);
}

void test_long_press_is_registered_before_release(void)
{
    bool should_service = true;
    btn->press();
    state->increment_time(Button::long_press_threshold_ms + 100, should_service);
    TEST_ASSERT_EVENTS(LONG_PRESS);
}

void test_single_press_too_slow_for_double(void)
{
    btn->press();
    state->set_time(Button::double_press_timeout_ms - 1);
    btn->release();

    state->set_time(Button::double_press_timeout_ms + 1);
    TEST_ASSERT_EVENTS(SINGLE_PRESS);
}

void test_press_twice(void)
{
    btn->press();
    state->increment_time(Button::double_press_timeout_ms / 2 + 1);
    btn->release();
    state->increment_time(Button::double_press_timeout_ms / 2 + 1);

    TEST_ASSERT_EVENTS(SINGLE_PRESS);

    btn->press();
    state->increment_time(Button::double_press_timeout_ms / 2 + 1);
    btn->release();
    state->increment_time(Button::double_press_timeout_ms / 2 + 1);

    TEST_ASSERT_EVENTS(SINGLE_PRESS, SINGLE_PRESS);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_single_press);
    RUN_TEST(test_double_press);
    RUN_TEST(test_long_press_is_registered_before_release);
    RUN_TEST(test_long_press_is_registered_on_release);
    RUN_TEST(test_long_press_isnt_triggered_when_rotating);
    RUN_TEST(test_press_is_triggered_before_rotation_when_pressing_and_then_rotating);
    RUN_TEST(test_press_isnt_triggered_when_holding_down_and_rotating);
    RUN_TEST(test_rotation_speed_is_forwarded);
    RUN_TEST(test_single_press_too_slow_for_double);
    RUN_TEST(test_press_twice);

    UNITY_END();

    return 0;
}
