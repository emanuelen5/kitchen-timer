#ifndef UART_H
#define UART_H

#include <stdint.h>

void init_UART(void);
void UART_print_string(const char* str);
void UART_printf(const char *format, ...);
void int_to_string(int16_t num, char *str);
void reverse_string(char *str);
char* UART_read_string();
void service_uart(void);

#endif // UART_H
