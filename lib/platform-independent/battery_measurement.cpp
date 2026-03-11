#include "battery_measure.h"

void init_battery_measurement(battery_measurement_t* measurement)
{
    measurement->centivolts_accumulator = 0;
    measurement->measurements_taken = 0;
}

void add_battery_measurement(battery_measurement_t* measurement, uint16_t centivolts)
{
    measurement->centivolts_accumulator += centivolts;
    measurement->measurements_taken++;
}

uint16_t get_average_battery_voltage(battery_measurement_t* measurement)
{
    if (measurement->measurements_taken == 0)
        return 0;
    return measurement->centivolts_accumulator / measurement->measurements_taken;
}

bool battery_measurement_is_complete(battery_measurement_t* measurement)
{
    return measurement->measurements_taken >= 16;
}
