#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#define CLK_PIN PD2 // INT0
#define DT_PIN PD3  // INT1
#define SW_PIN PD4

//Funtion pointer declaration
typedef void (*event_cb_t)(void);

void init_rotary_encoder(event_cb_t cw_rot_ptr, event_cb_t ccw_rot_ptr);

#endif // ROTARY_ENCODER_H
