#include "unity_config.h"
#include <avr/io.h>
#define BAUD 9600
#include <util/setbaud.h>
#include "util.h"
#include <util/delay.h>

static void init_uart(void)
{
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif

    UCSR0B = bit(TXEN0) | bit(RXEN0);   // Enable RX/TX
    UCSR0C = bit(UCSZ01) | bit(UCSZ00); // 8 data bits, 1 stop bit
}

static void transmit_flush(void)
{

    loop_until_bit_is_set(UCSR0A, UDRE0);
}

static void transmit_char(char c)
{
    transmit_flush();
    UDR0 = c;
}

void unity_output_start()
{
    init_uart();
    // Tests start too quickly for platform IO to recognize
    _delay_ms(1000);
}

void unity_output_char(char c)
{
    transmit_char(c);
}

void unity_output_flush()
{
    transmit_flush();
}

void unity_output_complete()
{
}
