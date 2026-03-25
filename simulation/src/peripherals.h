/*
 * peripherals.h
 *
 * Shared peripheral state between the simulator main and the graphics module.
 * Contains all the virtual peripheral instances used by the simulation.
 */

#ifndef __SIMULATION_PERIPHERALS_H__
#define __SIMULATION_PERIPHERALS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "max7219_virt.h"
#include "buzzer_virt.h"
#include "rotenc.h"
#include "button.h"

typedef struct peripherals_t {
    max7219_virt_t  display;
    buzzer_virt_t   buzzer;
    rotenc_t        rotary_encoder;
    /* Button is integrated into the rotary encoder via IRQ_ROTENC_OUT_BUTTON_PIN */
} peripherals_t;

#ifdef __cplusplus
}
#endif

#endif /* __SIMULATION_PERIPHERALS_H__ */
