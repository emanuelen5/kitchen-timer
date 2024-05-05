#include "stdint.h"

typedef struct
{
    uint16_t original_time, current_time;
} timer_t;

typedef void (*callBack_t)(void);

void init_timer(timer_t* timer,callBack_t second_tick_cb);
void change_original_time(timer_t *timer, int step);
void reset_original_time(timer_t *timer);
void increment_current_time(timer_t *timer);
bool current_time_is_finished(timer_t *timer);
uint16_t current_time_seconds_left(timer_t *timer);
