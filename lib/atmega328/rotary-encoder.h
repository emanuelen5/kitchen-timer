#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <stdint.h>

#define CLK_PIN PD2 // INT0
#define DT_PIN PD3  // INT1
#define SW_PIN PD4  //PCINT0

struct button
{
    bool pressed_down;
    uint16_t press_start_time_ms;
    uint8_t press_count;
};

//Funtion pointer declaration
typedef void (*event_cb_t)(void);

void init_rotary_encoder(event_cb_t cw_rotation_cb, event_cb_t ccw_rotation_cb, event_cb_t single_button_press_cb, event_cb_t double_button_press_cb, event_cb_t long_button_press_cb);
void service_button_press();

#endif // ROTARY_ENCODER_H
