#ifndef UART_H
#define UART_H

typedef void (*callback_t)(char *line);

void init_hw_UART(callback_t cb);
void service_receive_UART(void);

void UART_printf(const char *format, ...);

#endif // UART_H
