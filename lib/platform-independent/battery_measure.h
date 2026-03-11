#ifndef BATTERY_MEASURE_H
#define BATTERY_MEASURE_H

#include <stdint.h>

typedef struct 
{
    uint16_t centivolts_accumulator;
    uint8_t measurements_taken;
} battery_measurement_t;

void init_battery_measurement(battery_measurement_t* measurement);
void add_battery_measurement(battery_measurement_t* measurement, uint16_t centivolts);
uint16_t get_average_battery_voltage(battery_measurement_t* measurement);
bool battery_measurement_is_complete(battery_measurement_t* measurement);

#endif // BATTERY_MEASURE_H