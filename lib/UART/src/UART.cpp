#include "UART.h"
#define BAUD 9600               //QUESTION: why BAUD macro needs to be before the #include setbaud.h?
#include <util/setbaud.h>



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

void receive_buffer_has_data(void)
{
    loop_until_bit_is_set(UCSR0A, RXC0);
}

 uint8_t receive_byte(void)
{
    receive_buffer_has_data();
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
