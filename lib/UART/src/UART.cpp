#include "UART.h"
#define BAUD 9600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>
#include <stdarg.h>
#include <util.h>
#include "uint8-queue.h"

void init_UART(void)
{
    //Set baud rate
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    #if USE_2X
        UCSR0A |= (1 << U2X0);
    #else
        UCSR0A &= ~(1 << U2X0);
    #endif

    uint8_t sreg = SREG;
    cli();
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // Enable receiver, transmitter and receive interrupt
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);               // Set frame format: 8 data bits, 1 stop bit, no parity

    SREG = sreg;
}

uint8_queue_t tx_queue;
volatile const char* tx_string;
volatile uint8_t tx_index;

void UART_enable_transmit_interrupt()
{
    UCSR0B |= bit(UDRIE0);
}

void UART_desable_transmit_interrupt()
{
    UCSR0B &= ~(1 << UDRIE0);
}

void UART_print_char(const char c)
{
    char tx_char = c;
    add_to_queue(&tx_queue, (uint8_t)tx_char);
    UART_enable_transmit_interrupt();
}

void UART_print_string(const char* str)
{
    tx_string = str;
    tx_index = 0;
}

void service_transmit_UART(void)
{
    if (tx_string == nullptr)
    {
        return;
    }

    if (queue_is_full(&tx_queue))
    {
        //ERROR: Buffer overflow
        return;
    }
    else
    {
        UART_print_char(tx_string[tx_index++]);

        if(tx_string[tx_index] == '\0')
        {
            tx_index = 0;
            tx_string = nullptr;
            return;
        }
    }
}

ISR(USART_UDRE_vect)
{  
    dequeue_return_t tx_letter = dequeue(&tx_queue);
    UDR0 = tx_letter.value;

    if(queue_is_empty(&tx_queue))
    {
        UART_desable_transmit_interrupt();
    }
}

static void reverse_string(char *str) 
{
    int length = 0;
    while (str[length] != '\0')
    {
        length++;
    }

    int start = 0;
    int end = length -1;
    while(start < end)
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        
        start++;
        end--;
    }
}

static void int_to_string(int16_t num, char *str)
{
    int i = 0;
    bool is_negative = false;

    if (num < 0)
    {
        is_negative = true;
        num = -num;
    }

    do
    {
        str[i++] = num % 10 + '0';
        num = num / 10;
    } while (num != 0);

    if (is_negative)
    {
        str[i++] = '-';
    }
    str[i] = '\0';
    reverse_string(str);
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
                    int_to_string(num, num_str);
                    UART_print_string(num_str);
                    break;
                }
                case 'f': {
                    //double num = va_arg(args, double);
                    //TODO
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


uint8_queue_t rx_queue;
ISR(USART_RX_vect)
{
    char receivedChar = UDR0;
    add_to_queue(&rx_queue, (uint8_t)receivedChar);
}

#define RX_BUFFER_SIZE 64
static char rx_buffer[RX_BUFFER_SIZE];
static uint8_t rx_index = 0;

void service_receive_UART(void)
{
    if (queue_is_empty(&rx_queue))
    {
        return;
    }

    dequeue_return_t rx_letter = dequeue(&rx_queue);
    char character = rx_letter.value;
    if (rx_index < RX_BUFFER_SIZE - 1)
    {
        rx_buffer[rx_index++] = character;
        if (character == '\n' || character == '\r')
        {
            rx_buffer[rx_index] = '\0';
            rx_index = 0;
            UART_printf(rx_buffer);             //Debugging line
        }
    }
    else
    {
        //ERROR: Buffer overflow
        return;
    }
}

