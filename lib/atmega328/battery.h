#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

void init_hw_adc(void);
uint16_t battery_centivolts(uint8_t brightness);

#endif // BATTERY_H