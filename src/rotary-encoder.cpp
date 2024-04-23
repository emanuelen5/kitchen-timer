#include <Arduino.h>
#include "rotary-encoder.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static event_cb_t cw_rotation;
static event_cb_t ccw_rotation;


typedef struct {
    event_t data[QUEUE_SIZE];
    int front, rear;
} event_queue_t;

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
            queuing_interrupt(CW_INTERRUPT);
        else
            queuing_interrupt(CCW_INTERRUPT);
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
            queuing_interrupt(CCW_INTERRUPT);
        else
            queuing_interrupt(CW_INTERRUPT);;
    }
    last_trigger_INT1 = t;
}

void queuing_interrupt(int interrupt)
{
    // Create a new interrupt
    struct Interrupt newInterrupt;
    newInterrupt.type = interrupt;

    // Check if the queue is full
    if ((rear + 1) % QUEUE_SIZE != front) {
        // Add the interrupt to the queue
        interruptQueue[rear] = newInterrupt;
        rear = (rear + 1) % QUEUE_SIZE;
    } else {
        // Handle queue overflow (optional)
    }
}



void dequeuing_interrupt()
{
    if (event_queue_is_empty(&queue)) {
        return;
    }
        // Dequeue and process the next interrupt
        struct Interrupt currentInterrupt = interruptQueue[front];
        front = (front + 1) % QUEUE_SIZE;

        // Process the interrupt
        switch (currentInterrupt.type)
        {
            case CW_INTERRUPT:
                cw_rotation();
                break;

            case CCW_INTERRUPT:
                ccw_rotation();
                break;
        }
    }
}

