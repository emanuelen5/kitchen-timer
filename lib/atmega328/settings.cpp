
#include <avr/eeprom.h>
#include "settings.h"

void save_byte_setting(uint8_t *setting, eeprom_address address)
{
    eeprom_update_byte((uint8_t *)address, *setting);
}

void load_byte_setting(uint8_t *setting, eeprom_address address)
{
    *setting = eeprom_read_byte((uint8_t *)address);
}
