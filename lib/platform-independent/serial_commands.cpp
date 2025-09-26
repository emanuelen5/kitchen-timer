#include "serial_commands.h"
#include <string.h>
#include <stdio.h>
#include "config.h"

static void normalize_string_ending(char *str)
{
    size_t len = strlen(str);

    if(len == 0) return;

    if(str[len - 1] == '\n')
    {
        if(len >= 2 && str[len - 2] == '\r')
        {
            str[len - 2] = '\n';
            str[len - 1] = '\0';
        }
    } else if(str[len - 1] == '\r')
    {
        str[len - 1] = '\n';
    }
}

static bool parse_time_string(char* arg, uint32_t *steps)
{
    int hrs, mins, secs;
    uint8_t parsed_segments = sscanf(arg, "%d:%d:%d", &hrs, &mins, &secs);
    if(parsed_segments == 3)
    {
        *steps = hrs * 60 * 60 + mins * 60 + secs;
        return true;
    }
    else if (parsed_segments == 2)
    {
        secs = mins;
        mins = hrs;
        hrs = 0;
        *steps = mins * 60 + secs;
        return true;
    }
    else if (parsed_segments == 1)
    {
        secs = hrs;
        mins = 0;
        hrs = 0;
        *steps = secs;
        return true;
    }
    return false;
}


void handle_command(char* str, const command_callbacks_t* callbacks)
{

    normalize_string_ending(str);

    char *command = strtok(str, " \n");
    char *arg1 = strtok(NULL, " \n");
    char *arg2 = strtok(NULL, " \n");

    if(command == NULL) return;

    if(strcmp(command, "led") == 0)
    {
        if(arg1 && strcmp(arg1, "on") == 0)
        {
            callbacks->led_on();
        }
        else if(arg1 && strcmp(arg1, "off") == 0)
        {
            callbacks->led_off();
        }
    }
    else if(strcmp(command, "version") == 0)
    {
        callbacks->version();
    }
    else if(strcmp(command, "timer") == 0)
    {
        if(arg1 && strcmp(arg1, "set") == 0)
        {
            if(arg2)
            {
                uint32_t steps = 0;
                if(parse_time_string(arg2, &steps))
                {
                    callbacks->set_active_timer(&steps);
                }
            }
        }
        else if(arg1 && strcmp(arg1, "play") == 0)
        {
            callbacks->play_active_timer();
        }
        else if(arg1 && strcmp(arg1, "pause") == 0)
        {
            callbacks->pause_active_timer();
        }
        else if(arg1 && strcmp(arg1, "reset") == 0)
        {
            callbacks->reset_active_timer();
        }
        else if(arg1 && strcmp(arg1, "status") == 0)
        {
            callbacks->status_active_timer();
        }
    }
}
