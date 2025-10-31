#include <unity.h>
#include "serial_commands.h"

static bool led_on_called;
static bool led_off_called;
static bool version_called;
static bool set_active_timer_called;
static uint32_t last_timer_value;
static bool play_active_timer_called;
static bool pause_active_timer_called;
static bool reset_active_timer_called;
static bool get_status_active_timer_called;

void setUp(void)
{
    led_on_called = false;
    led_off_called = false;
    version_called = false;
    set_active_timer_called = false;
    last_timer_value = 0;
    play_active_timer_called = false;
    pause_active_timer_called = false;
    reset_active_timer_called = false;
    get_status_active_timer_called = false;
}

void tearDown(void) {}

void led_on(void)
{
    led_on_called = true;
}
void led_off(void)
{
    led_off_called = true;
}
void version(void)
{
    version_called = true;
}
void set_active_timer(uint32_t *steps)
{ 
    set_active_timer_called = true;
    last_timer_value = *steps;
}
void play_active_timer(void)
{
    play_active_timer_called = true;
}
void pause_active_timer(void)
{
    pause_active_timer_called = true;
}
void reset_active_timer(void)
{
    reset_active_timer_called = true;
}
void status_active_timer(void)
{
    get_status_active_timer_called = true;
}

static const command_callbacks_t callbacks = {
    .led_on = led_on,
    .led_off = led_off,
    .version = version,
    .set_active_timer = set_active_timer,
    .play_active_timer = play_active_timer,
    .pause_active_timer = pause_active_timer,
    .reset_active_timer = reset_active_timer,
    .status_active_timer = status_active_timer
};


void test_led_on_command(void)
{
    char cmd[] = "led on";
    handle_command(cmd, &callbacks);
    TEST_ASSERT_TRUE(led_on_called);
}

void test_led_off_command(void)
{
    char cmd[] = "led off";
    handle_command(cmd, &callbacks);
    TEST_ASSERT_TRUE(led_off_called);
}

void test_version_command(void)
{
    char cmd[] = "version";
    handle_command(cmd, &callbacks);
    TEST_ASSERT_TRUE(version_called);
}

void test_timer_set_command(void)
{
    char cmd[] = "timer set 01:02:03";
    handle_command(cmd, &callbacks);
    TEST_ASSERT_TRUE(set_active_timer_called);
    TEST_ASSERT_EQUAL_UINT32(3723, last_timer_value); // 1*3600 + 2*60 + 3
}

void test_timer_play_command(void)
{
    char cmd[] = "timer play";
    handle_command(cmd, &callbacks);
    TEST_ASSERT_TRUE(play_active_timer_called);
}

void test_timer_pause_command(void)
{
    char cmd[] = "timer pause";
    handle_command(cmd, &callbacks);
    TEST_ASSERT_TRUE(pause_active_timer_called);
}

void test_timer_reset_command(void)
{
    char cmd[] = "timer reset";
    handle_command(cmd, &callbacks);
    TEST_ASSERT_TRUE(reset_active_timer_called);
}

void test_timer_status_command(void)
{
    char cmd[] = "timer status";
    handle_command(cmd, &callbacks);
    TEST_ASSERT_TRUE(get_status_active_timer_called);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_led_on_command);
    RUN_TEST(test_led_off_command);
    RUN_TEST(test_version_command);
    RUN_TEST(test_timer_set_command);
    RUN_TEST(test_timer_play_command);
    RUN_TEST(test_timer_pause_command);
    RUN_TEST(test_timer_reset_command);
    RUN_TEST(test_timer_status_command);

    return UNITY_END();
}
