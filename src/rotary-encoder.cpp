#include <Arduino.h>
#include "led-counter.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rotary-encoder.h"

void cw_rotation(void);
void ccw_rotation(void);

volatile unsigned long last_trigger_INT0 = 0;
ISR(INT0_vect)
{
    unsigned long t = millis();
    if (t - last_trigger_INT0 > 70)
    {
        if (PIND & bit(CLK_PIN))
            cw_rotation();
        else
            ccw_rotation();
    }
    last_trigger_INT0 = t;
}

volatile unsigned long last_trigger_INT1 = 0;
ISR(INT1_vect)
{
    unsigned long t = millis();
    if (t - last_trigger_INT1 > 70)
    {
        if (PIND & bit(DT_PIN))
            ccw_rotation();
        else
            cw_rotation();
    }
    last_trigger_INT1 = t;
}

void init_rotary_encoder()
{
    DDRD &= 0;
    PORTD |= bit(SW_PIN) | bit(CLK_PIN) | bit(DT_PIN);

    cli();
    EIMSK |= bit(INT0) | bit(INT1);                              // Interrupt enable INT0 and INT1
    EICRA |= bit(ISC11) | bit(ISC10) | bit(ISC01) | ~bit(ISC00); // rising interrupt on INT1 and falling interrupt on INT0
    sei();
}
