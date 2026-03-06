#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdio.h>
#include <stdint.h>

/* 
IMPORTANT: eeprom addresses should not be moved to avoid
backward-compatibility issues
*/
enum eeprom_address
{
    EEPROM_BRIGHTNESS_ADDR = 0,
    EEPROM_VOLUME_ADDR = 1,
    EEPROM_MELODY_ADDR = 2
};

void save_byte_setting(uint8_t setting, enum eeprom_address address);
void load_byte_setting(uint8_t *setting, enum eeprom_address address);

#endif
