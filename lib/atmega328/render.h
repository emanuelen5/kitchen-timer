#ifndef RENDER_H
#define RENDER_H

#include "application.h"

typedef struct {
    uint16_t last_blink_time;
    bool blink_is_on;
} blink_state_t;

void render(application_t *app);

#endif // RENDER_H
