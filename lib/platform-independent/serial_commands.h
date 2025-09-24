#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>

typedef struct
{
    void (*led_on)(void);
    void (*led_off)(void);
    void (*version)(void);
    void (*set_active_timer)(uint32_t *steps);
} command_callbacks_t;

void handle_command(char* str, const command_callbacks_t* callbacks);

#endif
