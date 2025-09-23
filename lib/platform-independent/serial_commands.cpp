#include "serial_commands.h"
#include <string.h>
#include "config.h"

void handle_command(const char* cmd, const command_callbacks_t* callbacks)
{
    if (strncmp(cmd, "led on\n", RX_BUFFER_SIZE) == 0)
    {
        callbacks->led_on();
    }
    else if (strncmp(cmd, "led off\n", RX_BUFFER_SIZE) == 0)
    {
        callbacks->led_off();
    }
}
