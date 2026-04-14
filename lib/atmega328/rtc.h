#ifndef RTC_H
#define RTC_H

typedef void(callback(void));
void warm_up_oscillator_circuit(void);
void init_hw_timer2_to_1s_interrupt(callback *cb);

#endif // RTC_H
