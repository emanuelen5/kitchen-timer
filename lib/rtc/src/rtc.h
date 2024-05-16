#ifndef RTC_H
#define RTC_H

typedef void(callback(void));
void init_timer2_to_1s_interrupt(callback *cb);

#endif // RTC_H
