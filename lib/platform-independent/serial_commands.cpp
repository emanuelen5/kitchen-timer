#include "serial_commands.h"
#include <string.h>
#include "config.h"

static void normalize_string_ending(char *str)
{
    size_t len = strlen(str);

    if (len == 0) return;

    if (str[len - 1] == '\n')
    {
        if (len >= 2 && str[len - 2] == '\r')
        {
            str[len - 2] = '\n';
            str[len - 1] = '\0';
        }
    } else if (str[len - 1] == '\r')
    {
        str[len - 1] = '\n';
    }
}

void handle_command(char* cmd, const command_callbacks_t* callbacks)
{

    normalize_string_ending(cmd);

    if (strncmp(cmd, "led on\n", RX_BUFFER_SIZE) == 0)
    {
        callbacks->led_on();
    }
    else if (strncmp(cmd, "led off\n", RX_BUFFER_SIZE) == 0)
    {
        callbacks->led_off();
    }
}
