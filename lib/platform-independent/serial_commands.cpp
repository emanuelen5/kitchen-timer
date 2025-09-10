#include "serial_commands.h"
#include "str-helper.h"
#include "UART.h"

void handle_command(const char* cmd, command_callbacks_t* callbacks)
{
    if (str_equals(cmd, "led on\n"))
    {
        callbacks->led_on();
    }
    else if (str_equals(cmd, "led off\n"))
    {
        callbacks->led_off();
    }
}
