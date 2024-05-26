#include "stdint.h"

typedef struct
{
    uint16_t original_time, current_time;
} timer_t;

void change_original_time(timer_t *timer, int step);
void reset_timer(timer_t *timer);
void increment_current_time(timer_t *timer);
bool current_time_is_finished(timer_t *timer);
uint16_t current_time_seconds_left(timer_t *timer);