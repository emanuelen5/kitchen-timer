#include "battery.h"
#include <avr/io.h>
#include "util.h"

void init_hw_adc(void)
{
    ADMUX = bit(REFS1) | bit(REFS0) | bit(MUX2) | bit(MUX1) | bit(MUX0); // Vref=1.1V, ADC7
    ADCSRA = bit(ADEN) | bit(ADPS2) | bit(ADPS1) | bit(ADPS0);                        // Enable ADC, prescaler=128
}

// Voltage is scaled 1024 * 4.3 / 1.1
uint16_t battery_centivolts(void)
{
    ADCSRA |= bit(ADSC); // Start conversion
    while (ADCSRA & bit(ADSC))
        ;                     // Wait for conversion to complete
    uint16_t adc_value = ADC; // Read ADC value
    uint32_t voltage = (uint32_t)adc_value * 430 / 1024; // Scale to centivolts
    return (uint16_t)voltage;
}
