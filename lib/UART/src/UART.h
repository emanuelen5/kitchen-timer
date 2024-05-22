#ifndef UART_H
#define UART_H

#include <stdint.h>

void init_UART(void);
void service_uart(void);
void UART_print_string(const char* str);
void UART_printf(const char *format, ...);

#endif // UART_H
