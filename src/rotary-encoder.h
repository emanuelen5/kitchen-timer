#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#define CLK_PIN PD2 // INT0
#define DT_PIN PD3  // INT1
#define SW_PIN PD4

//Funtion pointer declaration
typedef void (*event_cb_t)(void);

void init_rotary_encoder(event_cb_t cw_rotation_cb, event_cb_t ccw_rotation_cb);

#endif
