#include "UART.h"
#define BAUD 9600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>
#include <stdarg.h>
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

    cli();
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);   // Enable receiver, transmitter and receive interrupt
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Set frame format: 8 data bits, 1 stop bit, no parity
    sei();
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

void service_uart(void)
{
    if (queue_is_empty(&rx_queue))
    {
        return;
    }

    dequeue_return_t letter = dequeue(&rx_queue);
    char character = letter.value;
    if (rx_index < RX_BUFFER_SIZE - 1)
    {
        rx_buffer[rx_index++] = character;
        if (character == '\n' || character == '\r')
        {
            rx_buffer[rx_index] = '\0';
            rx_index = 0;
        }
    }
    else
    {
        //ERROR: Buffer overflow
        return;
    }
}



static void transmit_buffer_is_ready(void)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);
}

static void transmit_byte(uint8_t data)
{
    transmit_buffer_is_ready();
    UDR0 = data;
}

void UART_print_string(const char* str)
{
    while(*str != '\0')
    {
        transmit_byte(*str);
        str++;
    }
}

void UART_printf(const char *format, ...)
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
            transmit_byte(*format);
        }
        format++;
    }

    va_end(args);
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

static void  reverse_string(char *str) 
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
