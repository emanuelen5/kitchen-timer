#ifndef UART_H
#define UART_H

#include "serial_commands.h"

void init_UART(command_callbacks_t command_cbs);
void service_receive_UART();

void UART_printf(const char *format, ...);

#endif // UART_H
