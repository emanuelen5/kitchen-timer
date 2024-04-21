#include <Arduino.h>
#include "rotary-encoder.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>



function_callback global_cw_rot_ptr;
function_callback global_ccw_rot_ptr;
int global_cw_event;
int global_ccw_event;

void init_rotary_encoder(function_callback cw_rot_ptr, function_callback ccw_rot_ptr, int cw_event, int ccw_event)
{
    DDRD &= 0;
    PORTD |= bit(SW_PIN) | bit(CLK_PIN) | bit(DT_PIN);

    cli();
    EIMSK |= bit(INT0) | bit(INT1);                              // Interrupt enable INT0 and INT1
    EICRA |= bit(ISC11) | bit(ISC10) | bit(ISC01) | ~bit(ISC00); // rising interrupt on INT1 and falling interrupt on INT0
    sei();

    global_cw_rot_ptr = cw_rot_ptr;
    global_ccw_rot_ptr = ccw_rot_ptr;
    global_cw_event = cw_event;
    global_ccw_event = ccw_event;

}

void cw_rotation()
{
    global_cw_rot_ptr(global_cw_event);
}

void ccw_rotation()
{
    global_ccw_rot_ptr(global_ccw_event);
}



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
