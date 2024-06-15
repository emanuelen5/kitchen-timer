#ifndef LIB_DISPLAY_H
#define LIB_DISPLAY_H

#include "max72xx.h"

void init_display(void);
void set_display_registers(max72xx_cmd_t cmds[4]);

#endif // LIB_DISPLAY_H
