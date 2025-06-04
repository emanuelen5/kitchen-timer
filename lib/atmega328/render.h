#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include "config.h"
#include "state-machine.h"
#include "fat_font.h"
#include "max72xx_matrix.h"
#include "millis.h"

void init_render();
void render_timer_view(state_machine_t* timers, uint8_t timer_count, uint8_t active_timer_index);

#endif // RENDER_H
