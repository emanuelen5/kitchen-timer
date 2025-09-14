#include "millis.h"
#include <avr/interrupt.h>
#include "buzzer.h"
#include "fur-elise.h"

Buzzer buzzer;

int main()
{
    init_millis();
    sei();
    int volume = 1;
    while (true)
    {
        if (buzzer.is_finished())
        {
            if (volume < Buzzer::default_volume)
            {
                volume+=3;
                buzzer.set_volume(volume);
                buzzer.start_melody(fur_elise, 0);
            }
            else
            {
                buzzer.stop();
                break;
            }
        }
        buzzer.service();
    }
    return 0;
}
