#include "settings_menu.h"
#include <stdio.h>
#include "settings.h"

void save_byte_setting(uint8_t setting, eeprom_address address);
void max72xx_set_intensity(uint8_t intensity_level);
