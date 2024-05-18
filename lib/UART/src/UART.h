#ifndef UART_H
#define UART_H

#include <stdint.h>

void init_UART(void);
void UART_print_string(const char* str);
void UART_printf(const char *format, ...);
void int_to_string(uint16_t num, char *str);
void reverse_string(char *str);
char* UART_read_string();

//TODO: UART function to receive strings (this might need an interrupt)

#endif // UART_H
