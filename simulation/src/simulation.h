#ifndef __SIMULATION_SIMULATION_H__
#define __SIMULATION_SIMULATION_H__

#include "stdint.h"

extern "C"
{
#include "sim_avr.h"
}

extern uint8_t port_c_state;
const char *exit_reason(avr_t *avr);

#endif // __SIMULATION_SIMULATION_H__