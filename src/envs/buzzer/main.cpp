#include "fur-elise.h"
#include "millis.h"
#include <avr/interrupt.h>

int main()
{
    init_millis();
    sei();
    play_fur_elise();
    return 0;
}
