#ifndef UART_H
#define UART_H

typedef void (*callback_t)(char *line);

void init_hw_UART(callback_t cb);
void service_receive_UART(void);

void UART_print_char(const char c);
void UART_printf(const char *format, ...);
// The argument must point to a string in PROGMEM
void UART_print_P(const char *str);

#endif // UART_H
