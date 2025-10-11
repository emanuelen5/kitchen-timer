#ifndef UART_H
#define UART_H

#include <stdint.h>

typedef void (*callback_t)(char *line);

void init_hw_UART(callback_t cb);
void service_receive_UART(void);
uint8_t UART_received_char_count(void);

void UART_printf(const char *format, ...);

#endif // UART_H
