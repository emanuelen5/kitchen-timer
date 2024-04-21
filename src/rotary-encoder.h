#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#define CLK_PIN PD2 // INT0
#define DT_PIN PD3  // INT1
#define SW_PIN PD4

//Funtion pointer declaration
typedef void (*function_callback)(int);

void init_rotary_encoder(function_callback cw_rot_ptr, function_callback ccw_rot_ptr, int cw_event, int ccw_event);


#endif
