#include "UART.h"

void init_UART(uint16_t baud_rate)
{
    //Set baud rate
    UBRR0H = (uint8_t)(baud_rate >> 8); 
    UBRR0L = (uint8_t)(baud_rate & 0xFF);;

    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void transmit_byte(uint8_t data)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);   //Wait until transmit buffer is ready to be written on.
    UDR0 = data;                            // Put data into buffer, sends the data
}

uint8_t receive_byte(void)
{
    loop_until_bit_is_set(UCSR0A, RXC0);       //Wait until there is unread data in receive buffer
    return UDR0;
}

void UART_print_string(const char* str)
{
    while(*str != '\0')
    {
        transmit_byte(*str);
        str++;
    }
}

