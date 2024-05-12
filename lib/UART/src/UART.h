#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <stdint.h>

void init_UART(uint16_t baud_rate);
void transmit_byte(uint8_t data);
uint8_t receive_byte(void);
void UART_print_string(const char *str);


#endif // UART_H
