#include "display.h"
#include "max72xx.h"

void set_display_registers(max72xx_cmd_t cmds[4])
{
    max72xx_send_commands(cmds, 4);
}

void init_display(void)
{
    init_max72xx();
}
