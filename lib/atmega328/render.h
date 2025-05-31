//Converts total_seconds to minutes/seconds or hours/minutes based on value
//Clears the matrix buffer
//Uses the fatfont_get_char(char) function to get 7x6 bitmaps
//Uses matrix_set_pixel(x, y, is_on) to draw digits
//Calls matrix_update() only if something changed

#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include "state-machine.h"

void render_timer_view(state_machine_t* timers, uint8_t timer_count, uint8_t active_timer_index);

#endif // RENDER_H
