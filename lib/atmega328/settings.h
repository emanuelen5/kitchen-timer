#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdio.h>

#define EEPROM_BRIGHTNESS_ADDR 0
#define EEPROM_VOLUME_ADDR 1

void save_int_setting(uint8_t *setting, uint8_t *address);
void load_int_setting(uint8_t *setting, uint8_t *address);

#endif
