#include <unity.h>
#include "serial_commands.h"
#include "application.h"

static bool led_on_called;
static bool version_called;
static bool set_active_timer_called;
static uint32_t last_timer_value;
static bool play_active_timer_called;
static bool pause_active_timer_called;
static bool reset_active_timer_called;
static bool get_status_active_timer_called;
static bool setup_brightness_called;
static uint8_t brightness_value;
static bool setup_volume_called;
static bool setup_status_called;
static bool help_called;
static bool unrecognized_command_called;
static bool reboot_called;

application_t app;

void setUp(void)
{
    led_on_called = false;
    version_called = false;
    set_active_timer_called = false;
    last_timer_value = 0;
    play_active_timer_called = false;
    pause_active_timer_called = false;
    reset_active_timer_called = false;
    get_status_active_timer_called = false;
    setup_brightness_called = false;
    setup_volume_called = false;
    setup_status_called = false;
    help_called = false;
    unrecognized_command_called = false;
    reboot_called = false;
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
        led_on_called = true;
    }
    else
    {
        led_on_called = false;
    }
}

void version(void)
{
    version_called = true;
}

void set_active_timer(state_machine_t *active_sm, uint32_t *steps)
{ 
    set_state(active_sm, SET_TIME);
    set_active_timer_called = true;
    last_timer_value = *steps;
}

void play_active_timer(state_machine_t *active_sm)
{
    if(get_state(active_sm) == SET_TIME)
    {
        play_active_timer_called = true;
    }
    else
    {
        play_active_timer_called = false;
    }
}

void pause_active_timer(state_machine_t *active_sm)
{
    if(get_state(active_sm) == SET_TIME)
    {
        pause_active_timer_called = true;
    }
    else
    {
        pause_active_timer_called = false;
    }
}

void reset_active_timer(state_machine_t *active_sm)
{
    if(get_state(active_sm) == SET_TIME)
    {
        reset_active_timer_called = true;
    }
    else
    {
        reset_active_timer_called = false;
    }
}

void get_status_active_timer(state_machine_t *active_sm)
{
    if(get_state(active_sm) == SET_TIME)
    {
        get_status_active_timer_called = true;
    }
    else
    {
        get_status_active_timer_called = false;
    }
}

void setup_brightness(uint8_t *intensity)
{
    if(*intensity > 16)
    {
        setup_brightness_called = false;
        return;
    }
    setup_brightness_called = true;
    brightness_value = *intensity;
}

void setup_volume(Buzzer* buzzer, uint8_t *volume)
{
    if(*volume > 11)
    {
        setup_volume_called = false;
        return;
    }
    setup_volume_called = true;
    buzzer->set_volume(*volume);
}

void setup_status(application_t *app)
{
    if(app->buzzer.get_volume() != 10)
    {
        setup_status_called = false;
        return;
    }
    setup_status_called = true;
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
    help_called = true;
}

void unrecognized_command(char *string)
{
    if(string == NULL)
    {
        return;
    }
    unrecognized_command_called = true;
}

void reboot(void)
{
    reboot_called = true;
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
    TEST_ASSERT_TRUE(led_on_called);
}

void test_test_led_off_command(void)
{
    char cmd[] = "test led off";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_FALSE(led_on_called);
}

void test_version_command(void)
{
    char cmd[] = "version";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(version_called);
}

void test_timer_set_command(void)
{
    char cmd[] = "timer set 01:02:03";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(set_active_timer_called);
    TEST_ASSERT_EQUAL_UINT32(3723, last_timer_value); // 1*3600 + 2*60 + 3
}

void test_timer_play_command(void)
{
    char cmd[] = "timer play";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(play_active_timer_called);
}

void test_timer_pause_command(void)
{
    char cmd[] = "timer pause";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(pause_active_timer_called);
}

void test_timer_reset_command(void)
{
    char cmd[] = "timer reset";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(reset_active_timer_called);
}

void test_timer_status_command(void)
{
    char cmd[] = "timer status";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(get_status_active_timer_called);
}

void test_setup_brightness_command(void)
{
    char cmd[] = "setup brightness 10";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(setup_brightness_called);
    TEST_ASSERT_EQUAL_UINT8(10, brightness_value);
}

void test_setup_volume_command(void)
{
    char cmd[] = "setup volume 10";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(setup_volume_called);
    TEST_ASSERT_EQUAL_UINT8(10, app.buzzer.get_volume());
}

void test_setup_status_command(void)
{
    char cmd[] = "setup status";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(setup_status_called);
    TEST_ASSERT_EQUAL_UINT8(10, brightness_value);
    TEST_ASSERT_EQUAL_UINT8(10, app.buzzer.get_volume());
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
    TEST_ASSERT_TRUE(help_called);
}

void test_unrecognized_command(void)
{
    char cmd[] = "asdf";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(unrecognized_command_called);
}

void test_reboot_command(void)
{
    char cmd[] = "reboot";
    handle_command(cmd, &callbacks, &app);
    TEST_ASSERT_TRUE(reboot_called);
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
