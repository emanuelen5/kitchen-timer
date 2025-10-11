#ifndef UART_H
#define UART_H

#include <stdint.h>

void init_hw_UART(void);
void service_receive_UART(void);
uint8_t UART_received_char_count(void);

void UART_printf(const char *format, ...);

#endif // UART_H
