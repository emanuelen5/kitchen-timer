#include "serial_commands_cbs.h"
#include <avr/pgmspace.h>
#include "led-counter.h"
#include "UART.h"
#include "max72xx.h"
#include <string.h>


void test_led(bool is_on)
{
    is_on ? set_counter(1) : set_counter(0);
}

void version(void)
{
    UART_print_P(PSTR("Kitchen Timer, version 1.0.0\n"));
    UART_print_P(PSTR("Authors: Erasmus Cedernaes, Nicolas Perozzi\n"));
}

void set_active_timer(state_machine_t *active_sm, uint32_t *steps)
{
    reset_timer(&active_sm->timer);
    set_state(active_sm, SET_TIME);
    change_original_time(&active_sm->timer, (int32_t*)steps);
}

void play_active_timer(state_machine_t *active_sm)
{
    if(get_state(active_sm) == SET_TIME)
    {
        state_machine_handle_event(active_sm, SINGLE_PRESS);
    }
    else
    {
        UART_print_P(PSTR("Timer has not SET_TIME mode.\n"));
    }
}
void pause_active_timer(state_machine_t *active_sm)
{
    if(get_state(active_sm) == RUNNING)
    {
        state_machine_handle_event(active_sm, SINGLE_PRESS);
    }
    else
    {
        UART_print_P(PSTR("Timer has not in RUNNING mode.\n"));
    }
}

void reset_active_timer(state_machine_t *active_sm)
{
    if(get_state(active_sm) != IDLE)
    {
        state_machine_handle_event(active_sm, LONG_PRESS);
    }
    else
    {
        UART_print_P(PSTR("Timer is in IDLE mode.\n"));
    }
}

static void convert_seconds_to_hhmmss(uint16_t seconds, uint8_t time[3])
{
    time[0] = seconds / 3600;
    time[1] = (seconds % 3600) / 60;
    time[2] = seconds % 60;
}

void get_status_active_timer(state_machine_t *active_sm)
{
    state_t current_state = get_state(active_sm);
    const char* current_state_string = state_to_string(&current_state);

    uint16_t current_seconds = get_current_time(active_sm);
    uint8_t current_time[3];
    convert_seconds_to_hhmmss(current_seconds, current_time);

    uint8_t current_hrs = current_time[0];
    uint8_t current_mins = current_time[1];
    uint8_t current_secs = current_time[2];

    uint16_t original_seconds = get_original_time(active_sm);
    uint8_t original_time[3];
    convert_seconds_to_hhmmss(original_seconds, original_time);

    uint8_t original_hrs = original_time[0];
    uint8_t original_mins = original_time[1];
    uint8_t original_secs = original_time[2];



    UART_printf("Current State: %s\n", current_state_string);
    if(current_state != IDLE && current_state != SET_TIME)
    {
        UART_printf("Timer set to: %02d:%02d:%02d\n", original_hrs, original_mins, original_secs);
        UART_printf("Current time: %02d:%02d:%02d\n", current_hrs, current_mins, current_secs);
    }
    else if(current_state == SET_TIME)
    {
        UART_printf("Timer set to: %02d:%02d:%02d\n", original_hrs, original_mins, original_secs);
    }
}

void setup_brightness(uint8_t *intensity)
{
    if(*intensity > 16)
    {
        UART_print_P(PSTR("The display brightness must be a value between 0 to 15.\n"));
        return;
    }

    max72xx_set_intensity(*intensity);
}

void setup_volume(Buzzer* buzzer, uint8_t *volume)
{
    if(*volume > 11)
    {
        UART_print_P(PSTR("The volume must be a value between 0 and 10.\n"));
    }
    buzzer->set_volume(*volume);
}

void setup_status(application_t *app)
{
    UART_printf("Volume: %d\n", app->buzzer.get_volume());
    UART_printf("Brightness: %d\n", get_intensity());
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
    buzzer->start_melody(beeps, 2);
}

const PROGMEM char help[] = (
    "KITCHEN_TIMER SERIAL COMMANDS:\n\n"
    "Usage:[command]... [argument]... [argument]...\n\n"
    "help\t\t\t\tDisplays descriptions of the available serial commands.\n"
    "version\t\t\t\tDisplayes information about the SW version and authors.\n"
    "setup\t\t\t\tCommand to setup kitchen_timer configurations.\n"
    "\tbrightness\t\tSetup display brightness from 0 to 15\n"
    "\tvolume\t\t\tSetup buzzer volume from 0 to 10.\n"
    "\tbuzzer on/off\t\tSetup the buzzer to volume 0.\n"
    "\tstatus\t\t\tDisplays information about the current device setup.\n"
    "test\t\t\t\tCommand to test kitchen_timer hardware.\n"
    "\tbuzzer\t\t\tTests the buzzer.\n"
    "\tled on/off\t\tTests the debugging led.\n"
    "timer\t\t\t\tCommand to operate the active_timer from the serial command line.\n"
    "\tset\t\t\tCommand to set the time for the active timer.\n"
    "\t\thh:mm:ss\tSets hh:hours, mm:minutes, ss:seconds.\n"
    "\t\tmm:ss\t\tSets mm:minutes, ss:seconds.\n"
    "\t\tss\t\tSets ss:seconds.\n"
    "\tplay\t\t\tStarts the active_timer.\n"
    "\tpause\t\t\tPauses the active_timer.\n"
    "\treset\t\t\tResets the active_timer.\n"
    "\tstatus\t\t\tDisplay the status of the active_timer."
);

void help_cmd(void)
{
    UART_print_P(help);
}

void unrecognized_command(char *string)
{
    if(string == NULL)
    {
        UART_printf("Error: Missing command.\n");
    }
    else
    {
        UART_printf("Error: Unknown command \"%s\".\n", string);
    }

}
