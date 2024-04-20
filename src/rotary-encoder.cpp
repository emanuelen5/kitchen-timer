#include <Arduino.h>
#include "led-counter.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rotary-encoder.h"

void init_rotary_encoder(void (*cw_rot_fcn)(int), void (*ccw_rot_fcn)(int), int cw_event, int ccw_event)
{
    DDRD &= 0;
    PORTD |= bit(SW_PIN) | bit(CLK_PIN) | bit(DT_PIN);

    cli();
    EIMSK |= bit(INT0) | bit(INT1);                              // Interrupt enable INT0 and INT1
    EICRA |= bit(ISC11) | bit(ISC10) | bit(ISC01) | ~bit(ISC00); // rising interrupt on INT1 and falling interrupt on INT0
    sei();

    cw_rot_ptr = cw_rot_fcn(cw_event);
    ccw_rot_ptr = ccw_rot_fcn(ccw_event);

}


volatile unsigned long last_trigger_INT0 = 0;
ISR(INT0_vect)
{
    unsigned long t = millis();
    if (t - last_trigger_INT0 > 70)
    {
        if (PIND & bit(CLK_PIN))
            cw_rot_ptr;
        else
            ccw_rot_ptr;
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
            ccw_rot_ptr;
        else
            cw_rot_ptr;
    }
    last_trigger_INT1 = t;
}
