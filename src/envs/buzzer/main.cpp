#include "millis.h"
#include <avr/interrupt.h>
#include "buzzer.h"
#include "fur-elise.h"

Buzzer buzzer;

int main()
{
    init_millis();
    sei();
    buzzer.start_melody(fur_elise, 0);
    while (true) {
        buzzer.service();
    }
    return 0;
}
