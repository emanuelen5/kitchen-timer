#ifndef UART_H
#define UART_H

void init_hw_UART(void);
void service_receive_UART(void);

void UART_printf(const char *format, ...);

#endif // UART_H
