#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

void minimize_battery_voltage_jitter(void);
uint16_t battery_centivolts(void);

#endif // BATTERY_H