#include "stdint.h"

typedef struct
{
    uint16_t original_time, current_time;
} timer_t;

void change_timer(timer_t *timer, int step);
void reset_timer(timer_t *timer);
void decrement_timer(timer_t *timer);
bool timer_is_finished(timer_t *timer);
uint16_t timer_get_seconds_left(timer_t *timer);
