
#include <avr/eeprom.h>
#include "settings.h"

void save_int_setting(uint8_t *setting, uint8_t *address)
{
    eeprom_write_byte(address, *setting);
}

void load_int_setting(uint8_t *setting, uint8_t *address)
{
    *setting = eeprom_read_byte(address);
}
