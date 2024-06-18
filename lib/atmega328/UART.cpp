#include "UART.h"
#define BAUD 9600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>
#include <stdarg.h>
#include <util.h>
#include "uint8-queue.h"
#include "str-helper.h"

uint8_queue_t rx_queue = {};
static const uint8_t rx_queue_size = 64;
static uint8_t rx_queue_buffer[rx_queue_size];

uint8_queue_t tx_queue = {};
static const uint8_t tx_queue_size = 64;
static uint8_t tx_queue_buffer[tx_queue_size];

void init_UART(void)
{
    //Set baud rate
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= bit(U2X0);
#else
    UCSR0A &= ~bit(U2X0);
#endif

    uint8_t sreg = SREG;
    cli();
    UCSR0B = bit(RXEN0) | bit(TXEN0) | bit(RXCIE0); // Enable rx/tx; and rx/tx interrupt
    UCSR0C = bit(UCSZ01) | bit(UCSZ00);             // Set frame format: 8 data bits, 1 stop bit, no parity

    init_queue(&tx_queue, tx_queue_buffer, tx_queue_size);
    init_queue(&rx_queue, rx_queue_buffer, rx_queue_size);

    SREG = sreg;
}

static void inline enable_and_trigger_tx_interrupt(void)
{
    UCSR0B |= bit(UDRIE0);
}

static void inline disable_tx_interrupt(void)
{
    UCSR0B &= ~bit(UDRIE0);
}

void UART_print_char(const char c)
{
    while (queue_is_full(&tx_queue))
        ;

    add_to_queue(&tx_queue, (uint8_t)c);
    enable_and_trigger_tx_interrupt();
}

void UART_print_string(const char *str)
{
    while (*str != '\0')
    {
        UART_print_char(*str);
        str++;
    }
}

ISR(USART_UDRE_vect)
{
    dequeue_return_t tx_letter = dequeue(&tx_queue);
    if (!tx_letter.is_valid)
    {
        disable_tx_interrupt();
        return;
    }

    UDR0 = tx_letter.value;
}

void UART_printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    while (*format != '\0')
    {
        if (*format == '%')
        {
            format++;
            switch (*format)
            {
                case 'd': {
                    uint16_t num = va_arg(args, uint16_t);
                    char num_str[12];
                    write_int_into_string(num, num_str);
                    UART_print_string(num_str);
                    break;
                }
                case 's': {
                    const char *str = va_arg(args, const char *);
                    UART_print_string(str);
                    break;
                }
                default:
                    UART_print_string("Unsupported format specifier");
                    break;
            }
        }
        else
        {
            UART_print_char(*format);
        }
        format++;
    }
    va_end(args);
}

ISR(USART_RX_vect)
{
    char receivedChar = UDR0;
    add_to_queue(&rx_queue, (uint8_t)receivedChar);
}

#define RX_BUFFER_SIZE 64
static char rx_buffer[RX_BUFFER_SIZE] = {0};
static uint8_t rx_index = 0;

void service_receive_UART(void)
{
    if (queue_is_empty(&rx_queue))
        return;

    dequeue_return_t entry = dequeue(&rx_queue);
    char c = entry.value;

    if (rx_index >= RX_BUFFER_SIZE - 1)
        return; // ERROR: Buffer overflow

    rx_buffer[rx_index] = c;
    rx_index++;

    bool message_has_finished = c == '\n' || c == '\r';
    if (message_has_finished)
    {
        rx_buffer[rx_index] = '\0';
        rx_index = 0;
        UART_printf(rx_buffer);
    }
}

