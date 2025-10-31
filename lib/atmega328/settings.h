#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdio.h>

enum eeprom_address
{
    EEPROM_BRIGHTNESS_ADDR = 0,
    EEPROM_VOLUME_ADDR = 1
};

void save_byte_setting(uint8_t *setting, eeprom_address address);
void load_byte_setting(uint8_t *setting, eeprom_address address);

#endif
