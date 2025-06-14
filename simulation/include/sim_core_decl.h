#ifndef __SIM_CORE_DECL_H__
#define __SIM_CORE_DECL_H__

#include "sim_avr.h"

extern avr_kind_t mega328;

extern avr_kind_t *avr_kind[];
avr_kind_t *avr_kind[] = {
    &mega328,
    NULL,
};

#endif // __SIM_CORE_DECL_H__
