#ifndef __SIMULATION_GRAPHICS_H__
#define __SIMULATION_GRAPHICS_H__

extern "C"
{
#include "sim_avr.h"
}

#include "peripherals.h"

void simulate_with_graphics(avr_t *avr, peripherals_t *peripherals);

#endif // __SIMULATION_GRAPHICS_H__