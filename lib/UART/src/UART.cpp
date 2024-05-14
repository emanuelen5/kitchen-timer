#include "UART.h"

#define BAUD 9600
#include <util/setbaud.h>

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
