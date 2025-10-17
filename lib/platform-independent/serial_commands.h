#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>
#include <state-machine.h>

typedef struct
{
    void (*test_led)(bool is_on);
    void (*version)(void);
    void (*set_active_timer)(state_machine_t *active_sm, uint32_t *steps);
    void (*play_active_timer)(void);
    void (*pause_active_timer)(void);
    void (*reset_active_timer)(void);
    void (*status_active_timer)(void);
    void (*setup_brightness)(uint8_t *intensity);
    void (*setup_volume)(uint8_t * volume);
    void (*setup_status)(void);
    void (*setup_buzzer)(bool is_on);
    void (*test_buzzer)(void);
    void (*help_cmd)(void);
} command_callbacks_t;

void handle_command(char* str, const command_callbacks_t* callbacks, state_machine_t *active_sm);

#endif
