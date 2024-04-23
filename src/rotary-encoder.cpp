#include <Arduino.h>
#include "rotary-encoder.h"
#include "event_queue.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static event_cb_t cw_rotation;
static event_cb_t ccw_rotation;

void init_rotary_encoder(event_cb_t cw_rotation_cb, event_cb_t ccw_rotation_cb)
{
    DDRD &= 0;
    PORTD |= bit(SW_PIN) | bit(CLK_PIN) | bit(DT_PIN);

    cli();
    EIMSK |= bit(INT0) | bit(INT1);                              // Interrupt enable INT0 and INT1
    EICRA |= bit(ISC11) | bit(ISC10) | bit(ISC01) | ~bit(ISC00); // rising interrupt on INT1 and falling interrupt on INT0
    sei();

    cw_rotation = cw_rotation_cb;
    ccw_rotation = ccw_rotation_cb;
}


volatile unsigned long last_trigger_INT0 = 0;
ISR(INT0_vect)
{
    unsigned long t = millis();
    if (t - last_trigger_INT0 > 70)
    {
        if (PIND & bit(CLK_PIN))
            queuing_event(CW_INTERRUPT);
        else
            queuing_event(CCW_INTERRUPT);
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
            queuing_event(CCW_INTERRUPT);
        else
            queuing_event(CW_INTERRUPT);;
    }
    last_trigger_INT1 = t;
}

