#ifndef RENDER_H
#define RENDER_H

#include "state-machine.h"

void init_render();
void render_active_timer_view(state_machine_t* active_sm, uint8_t active_timer_index);

#endif // RENDER_H
