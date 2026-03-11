#include "boost_converter.h"
#include <avr/io.h>
#include "util.h"

void bc_power_safe_off(void)
{
    PORTC |= bit(PC3);
}

void bc_power_safe_on(void)
{
    PORTC &= ~bit(PC3);
}

void init_hw_boost_converter(void)
{
    DDRC |= bit(PC3);
    bc_power_safe_off();
}
