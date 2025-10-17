#ifndef SERIAL_COMMANDS_CBS_H
#define SERIAL_COMMANS_CBS_H

#include "state-machine.h"

void version(void);
void test_led(bool is_on);
void set_active_timer(state_machine_t *active_sm, uint32_t *steps);



#endif //SERIAL_COMMANDS_CBS_H
