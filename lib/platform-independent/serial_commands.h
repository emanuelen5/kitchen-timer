#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>

typedef struct
{
    void (*led_on)(void);
    void (*led_off)(void);
} command_callbacks_t;

void handle_command(char* cmd, const command_callbacks_t* callbacks);

#endif
