#ifndef UART_H
#define UART_H

#include <stdint.h>

void init_UART(void);
void service_receive_UART(void);
void service_transmit_UART(void);

void UART_printf(const char *format, ...);

#endif // UART_H
