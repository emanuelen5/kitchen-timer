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

void handle_command(char* str, const command_callbacks_t* callbacks)
{

    normalize_string_ending(str);

    char *command = strtok(str, " \n");
    char *arg1    = strtok(NULL, " \n");

    if (command == NULL) return;

    if (strcmp(command, "led") == 0)
    {
        if (arg1 && strcmp(arg1, "on") == 0) {
            callbacks->led_on();
        }
        else if (arg1 && strcmp(arg1, "off") == 0) {
            callbacks->led_off();
        }
    }
    else if (strcmp(command, "version") == 0)
    {
        callbacks->version();
    }
}
