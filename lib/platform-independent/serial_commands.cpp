#include "serial_commands.h"
#include "str-helper.h"
#include "UART.h"

void handle_command(const char* cmd, command_callbacks_t* callbacks)
{
    if (strings_are_equal(cmd, "led on\n"))
    {
        callbacks->led_on();
    }
    else if (strings_are_equal(cmd, "led off\n"))
    {
        callbacks->led_off();
    }
}
