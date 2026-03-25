#ifndef __SIMULATION_SIMULATION_H__
#define __SIMULATION_SIMULATION_H__

#include "stdint.h"

extern "C"
{
#include "sim_avr.h"
}

#include "peripherals.h"

extern uint8_t port_c_state;
extern peripherals_t g_sim_peripherals;
const char *exit_reason(avr_t *avr);
const char *exit_reason_bootloader(avr_t *avr);

#endif // __SIMULATION_SIMULATION_H__