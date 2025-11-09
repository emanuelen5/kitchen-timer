#ifndef RENDER_H
#define RENDER_H

#include "state-machine.h"

typedef struct {
    uint16_t last_blink_time;
    bool blink_is_on;
} blink_state_t;

void render_active_timer_view(state_machine_t* active_sm, uint8_t active_timer_index);
void draw_char(char c, uint8_t x_offset, uint8_t y_offset, bool clear_digit);

#endif // RENDER_H
