#ifndef SERIAL_COMMANDS_CBS_H
#define SERIAL_COMMANS_CBS_H

#include "serial_commands.h"

void version(void);
void test_led(bool is_on);
void set_active_timer(state_machine_t *active_sm, uint32_t *steps);
void play_active_timer(state_machine_t *active_sm);
void pause_active_timer(state_machine_t *active_sm);
void reset_active_timer(state_machine_t *active_sm);
void get_status_active_timer(state_machine_t *active_sm);
void setup_brightness(uint8_t *intensity);
void setup_volume(Buzzer* buzzer, uint8_t *volume);
void setup_status(application_t *app);
void setup_buzzer(Buzzer* buzzer, bool is_on);
void test_buzzer(Buzzer *buzzer);
void help_cmd(void);

#endif //SERIAL_COMMANDS_CBS_H
