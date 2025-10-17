#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>
#include "application.h"

typedef struct
{
    void (*test_led)(bool is_on);
    void (*version)(void);
    void (*set_active_timer)(state_machine_t *active_sm, uint32_t *steps);
    void (*play_active_timer)(state_machine_t *active_sm);
    void (*pause_active_timer)(state_machine_t *active_sm);
    void (*reset_active_timer)(state_machine_t *active_sm);
    void (*status_active_timer)(state_machine_t *active_sm);
    void (*setup_brightness)(uint8_t *intensity);
    void (*setup_volume)(Buzzer *buzzer, uint8_t *volume);
    void (*setup_status)(application_t *app);
    void (*setup_buzzer)(Buzzer *buzzer, bool is_on);
    void (*test_buzzer)(Buzzer *buzzer);
    void (*help_cmd)(void);
} command_callbacks_t;

extern const command_callbacks_t command_callbacks;

void handle_command(char* str, const command_callbacks_t* callbacks, application_t *app);

#endif
