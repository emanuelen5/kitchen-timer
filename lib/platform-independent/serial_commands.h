#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>

typedef struct
{
    void (*led_on)(void);
    void (*led_off)(void);
    void (*version)(void);
    void (*set_active_timer)(uint32_t *steps);
    void (*play_active_timer)(void);
    void (*pause_active_timer)(void);
    void (*reset_active_timer)(void);
    void (*status_active_timer)(void);
    void (*setup_brightness)(uint8_t *intensity);
    void (*setup_volume)(uint8_t * volume);
} command_callbacks_t;

void handle_command(char* str, const command_callbacks_t* callbacks);

#endif
