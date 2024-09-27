#include "avr_button.h"

void AvrButton::service()
{
    uint8_t oldSREG = SREG;
    cli();

    Button::service();

    SREG = oldSREG;
}