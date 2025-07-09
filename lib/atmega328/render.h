#ifndef RENDER_H
#define RENDER_H

#include "state-machine.h"

void init_render();
void render_active_timer_view(state_machine_t* timers, uint8_t active_timer_index);
//TODO: void render_bars_view()

void blink_active_timer_indicator(uint8_t active_timer_index);

#endif // RENDER_H
