#include "battery.h"
#include <avr/io.h>
#include "util.h"
#include "max72xx.h"
#include <util/delay.h>

// Voltage is scaled 1024 * 4.3 / 1.1
uint16_t battery_centivolts(uint8_t brightness)
{
    ADMUX = bit(REFS1) | bit(REFS0) | bit(MUX2) | bit(MUX1) | bit(MUX0); // Vref=1.1V, ADC7
    ADCSRA = bit(ADEN) | bit(ADPS2) | bit(ADPS1) | bit(ADPS0);           // Enable ADC, prescaler=128
    max72xx_set_intensity(0);                                            // Minimize display load during ADC
    _delay_ms(2);                                                        // Allow voltage to stabilize
    ADCSRA |= bit(ADSC);                                                 // Start conversion
    while (ADCSRA & bit(ADSC))
        ;                                                    // Wait for conversion to complete
    uint16_t adc_value = ADC;                                // Read ADC value
    uint32_t voltage = (uint32_t)adc_value * 11 * 43 / 1024; // Scale to centivolts
    max72xx_set_intensity(brightness);                       // Restore brightness
    ADCSRA = 0;
    return (uint16_t)voltage;
}
