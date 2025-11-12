#include <unity.h>
#include "serial_commands.h"
#include "application.h"
#include <string.h>

struct 
{
    bool led_on_called;
    bool version_called;
    bool set_active_timer_called;
    uint32_t last_timer_value;
    bool play_active_timer_called;
    bool pause_active_timer_called;
    bool reset_active_timer_called;
    bool get_status_active_timer_called;
    bool setup_brightness_called;
    uint8_t brightness_value;
    bool setup_volume_called;
    bool setup_status_called;
    bool help_called;
    bool unrecognized_command_called;
    char unrecognized_command_str[100];
    bool reboot_called;

} test_state;


application_t app;

void setUp(void)
{
    test_state = {};
}

void tearDown(void) {}

unsigned long millis()
{
    static unsigned long t = 0;
    return t += 10;
}
void toneAC(unsigned long, unsigned char) {}
void noToneAC() {}

void init_application(application_t *app)
{
    app->brightness = 10;
    app->buzzer.set_volume(10);
    app->current_active_sm = 0;
    set_state(&app->state_machines[0], SET_TIME);
}

void test_led(bool is_on)
{
    if (is_on)
    {
        test_state.led_on_called = true;
    }
    else
    {
        test_state.led_on_called = false;
    }
}

void version(void)
{
    test_state.version_called = true;
}

void set_active_timer(state_machine_t *active_sm, uint32_t *steps)
{ 
    set_state(active_sm, SET_TIME);
    test_state.set_active_timer_called = true;
    test_state.last_timer_value = *steps;
}

void play_active_timer(state_machine_t *active_sm)
{
    (void)active_sm;
    test_state.play_active_timer_called = true;
}

void pause_active_timer(state_machine_t *active_sm)
{
    (void)active_sm;
    test_state.pause_active_timer_called = true;
}

void reset_active_timer(state_machine_t *active_sm)
{
    (void)active_sm;
    test_state.reset_active_timer_called = true;
}

void get_status_active_timer(state_machine_t *active_sm)
{
    (void)active_sm;
    test_state.get_status_active_timer_called = true;
}

void setup_brightness(uint8_t *intensity)
{
    test_state.brightness_value = *intensity;
    test_state.setup_brightness_called = true;
}

void setup_volume(Buzzer* buzzer, uint8_t *volume)
{
    (void)buzzer;
    (void)volume;
    test_state.setup_volume_called = true;
}

void setup_status(application_t *app)
{
    (void)app;
    test_state.setup_status_called = true;
}

void setup_buzzer(Buzzer* buzzer, bool is_on)
{
    if(is_on)
    {
        buzzer->set_volume(buzzer->default_volume);
    }
    else
    {
        buzzer->set_volume(0);
    }
}

void test_buzzer(Buzzer *buzzer)
{
    buzzer->set_volume(5);
}

void help_cmd(void)
{
    test_state.help_called = true;
}

void unrecognized_command(char *string)
{
    test_state.unrecognized_command_called = true;
    strncpy(test_state.unrecognized_command_str, string, sizeof(test_state.unrecognized_command_str));
}

void reboot(void)
{
    test_state.reboot_called = true;
}

static const command_callbacks_t callbacks =
{
    .test_led = test_led,
    .version = version,
    .set_active_timer = set_active_timer,
    .play_active_timer = play_active_timer,
    .pause_active_timer = pause_active_timer,
    .reset_active_timer = reset_active_timer,
    .status_active_timer = get_status_active_timer,
    .setup_brightness = setup_brightness,
    .setup_volume = setup_volume,
    .setup_status = setup_status,
    .setup_buzzer = setup_buzzer,
    .test_buzzer = test_buzzer,
    .help = help_cmd,
    .unrecognized_command = unrecognized_command,
    .reboot = reboot
};


void test_test_led_on_command(void)
{
    char cmd[] = "test led on";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(test_state.led_on_called);
}

void test_test_led_off_command(void)
{
    char cmd[] = "test led off";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_FALSE(test_state.led_on_called);
}

void test_version_command(void)
{
    char cmd[] = "version";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(test_state.version_called);
}

void test_timer_set_command(void)
{
    char cmd[] = "timer set 01:02:03";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(test_state.set_active_timer_called);
    TEST_ASSERT_EQUAL_UINT32(3723, test_state.last_timer_value); // 1*3600 + 2*60 + 3
}

void test_timer_play_command(void)
{
    char cmd[] = "timer play";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(test_state.play_active_timer_called);
}

void test_timer_pause_command(void)
{
    char cmd[] = "timer pause";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(test_state.pause_active_timer_called);
}

void test_timer_reset_command(void)
{
    char cmd[] = "timer reset";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(test_state.reset_active_timer_called);
}

void test_timer_status_command(void)
{
    char cmd[] = "timer status";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(test_state.get_status_active_timer_called);
}

void test_setup_brightness_command(void)
{
    char cmd[] = "setup brightness 10";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(test_state.setup_brightness_called);
    TEST_ASSERT_EQUAL_UINT8(10, test_state.brightness_value);
}

void test_setup_volume_command(void)
{
    char cmd[] = "setup volume 10";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(test_state.setup_volume_called);
    TEST_ASSERT_EQUAL_UINT8(10, app.buzzer.get_volume());
}

void test_setup_status_command(void)
{
    char cmd[] = "setup status";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(test_state.setup_status_called);
}

void test_setup_buzzer_command(void)
{
    char cmd1[] = "setup buzzer off";
    handle_command(cmd1, &callbacks, &app);
    TEST_ASSERT_EQUAL_UINT8(0, app.buzzer.get_volume());

    char cmd2[] = "setup buzzer on";
    handle_command(cmd2, &callbacks, &app);
    TEST_ASSERT_EQUAL_UINT8(10, app.buzzer.get_volume());
}

void test_test_buzzer_command(void)
{
    char cmd[] = "test buzzer";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_EQUAL_UINT8(5, app.buzzer.get_volume());
}

void test_help_command(void)
{
    char cmd[] = "help";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(test_state.help_called);
}

void test_unrecognized_command(void)
{
    char cmd[] = "asdf";
    handle_command(cmd, &callbacks, &app);
    cmd[0] = '\0';
    TEST_ASSERT_TRUE(test_state.unrecognized_command_called);
    TEST_ASSERT_EQUAL_STRING("asdf", test_state.unrecognized_command_str);
}

void test_reboot_command(void)
{
    char cmd[] = "reboot";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(test_state.reboot_called);
}

int main(void) {
    UNITY_BEGIN();

    init_application(&app);

    RUN_TEST(test_test_led_on_command);
    RUN_TEST(test_test_led_off_command);
    RUN_TEST(test_version_command);
    RUN_TEST(test_timer_set_command);
    RUN_TEST(test_timer_play_command);
    RUN_TEST(test_timer_pause_command);
    RUN_TEST(test_timer_reset_command);
    RUN_TEST(test_timer_status_command);
    RUN_TEST(test_setup_brightness_command);
    RUN_TEST(test_setup_volume_command);
    RUN_TEST(test_setup_status_command);
    RUN_TEST(test_setup_buzzer_command);
    RUN_TEST(test_test_buzzer_command);
    RUN_TEST(test_help_command);
    RUN_TEST(test_unrecognized_command);
    RUN_TEST(test_reboot_command);

    return UNITY_END();
}
