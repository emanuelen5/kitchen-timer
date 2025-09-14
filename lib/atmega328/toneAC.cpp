/* ---------------------------------------------------------------------------
Created by Tim Eckel - teckel@leethost.com
Copyright 2019 License: GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html

See "toneAC.h" for purpose, syntax, version history, links, and more.
--------------------------------------------------------------------------- */

#include "toneAC.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#include "util.h"

uint8_t _tAC_volume[] = {200, 100, 67, 50, 40, 33, 29, 22, 11, 2}; // Duty for linear volume control.

void toneAC(unsigned long frequency, uint8_t volume)
{
    if (frequency == NOTONEAC || volume == 0)
    {
        noToneAC();
        return;
    } // If frequency or volume are 0, turn off sound and return.
    if (volume > 10)
        volume = 10; // Make sure volume is in range (1 to 10).

    toneAC_playNote(frequency, volume); // Routine that plays the note using timers.
}

void toneAC_playNote(unsigned long frequency, uint8_t volume)
{
    DDRB |= bit(DDB1) | bit(DDB2); // Set timer 1 PWM pins to OUTPUT (because analogWrite does it too).

    uint8_t prescaler = bit(CS10);                 // Try using prescaler 1 first.
    unsigned long top = F_CPU / frequency / 2 - 1; // Calculate the top.
    if (top > 65535)
    {                          // If not in the range for prescaler 1, use prescaler 256 (122 Hz and lower @ 16 MHz).
        prescaler = bit(CS12); // Set the 256 prescaler bit.
        top = top / 256 - 1;   // Calculate the top using prescaler 256.
    }

    ICR1 = top; // Set the top.
    if (TCNT1 > top)
        TCNT1 = top;                                  // Counter over the top, put within range.
    TCCR1B = bit(WGM13) | prescaler;                  // Set PWM, phase and frequency corrected (top=ICR1) and prescaler.
    OCR1A = OCR1B = top / _tAC_volume[volume - 1];    // Calculate & set the duty cycle (volume).
    TCCR1A = bit(COM1A1) | bit(COM1B1) | bit(COM1B0); // Inverted/non-inverted mode (AC).
}

void noToneAC()
{
    TIMSK1 &= ~bit(OCIE1A); // Remove the timer interrupt.
    TCCR1B = bit(CS11);     // Default clock prescaler of 8.
    TCCR1A = bit(WGM10);    // Set to defaults so PWM can work like normal (PWM, phase corrected, 8bit).
    PORTB &= ~bit(DDB1);    // Set timer 1 PWM pins to LOW.
    PORTB &= ~bit(DDB2);    // Other timer 1 PWM pin also to LOW.
}
