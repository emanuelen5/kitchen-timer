#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <stdint.h>

 void init_UART(void);
 void UART_print_string(const char *str);

//TODO: UART function to print combination of string and variables
//TODO: UART function to receive strings (this might need an interrupt)

#endif // UART_H
