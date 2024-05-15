#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <stdint.h>
#define BAUD 9600               //QUESTION: why BAUD macro needs to be before the #include setbaud.h?
#include <util/setbaud.h>
#include <stdarg.h>

 void init_UART(void);
  void UART_print_string(const char* str);
 void UART_print(const char *format, ...);
 void int_to_string(uint16_t num, char *str);
 void reverse_string(char *str);
 char* UART_read_string();

//TODO: UART function to receive strings (this might need an interrupt)

#endif // UART_H
