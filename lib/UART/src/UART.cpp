#include "UART.h"

 void init_UART(void)           //QUESTION: shouldn´t we make the UART able to accept different baud rates?
{
    //Set baud rate
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    #if USE_2X                      //QUESTION: Are we using this part?
        UCSR0A |= (1 << U2X0);
    #else
        UCSR0A &= ~(1 << U2X0);
    #endif

    UCSR0B = (1 << RXEN0) | (1 << TXEN0);   // Enable receiver and transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Set frame format: 8 data bits, 1 stop bit, no parity
}

void transmit_buffer_is_ready(void)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);
}

void transmit_byte(uint8_t data)
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

void UART_print(const char *format, ...)
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
                    double num = va_arg(args, double);
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

void int_to_string(uint16_t num, char *str)
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

void reverse_string(char *str) 
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

void receive_buffer_has_data(void)
{
    loop_until_bit_is_set(UCSR0A, RXC0);
}

 uint8_t receive_byte(void)
{
    receive_buffer_has_data();
    return UDR0;
}

#define READ_BUFFER_SIZE 255

char* UART_read_string()
{
    static char buffer[READ_BUFFER_SIZE];
    uint8_t i = 0;
    char receivedChar;

    while (i < (READ_BUFFER_SIZE - 1))
    {
        receivedChar = receive_byte();

        if (receivedChar == '\n' || receivedChar == '\r' || receivedChar == '\0')
        {
            break;
        }

        buffer[i++] = receivedChar;
    }

    buffer[i] = '\0';

    return buffer;
}
