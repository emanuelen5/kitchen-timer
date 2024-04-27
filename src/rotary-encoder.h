#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#define CLK_PIN PD2 // INT0
#define DT_PIN PD3  // INT1
#define SW_PIN PD4

typedef void (*rotationHandlerFunction)(void);

void init_rotary_encoder(rotationHandlerFunction cw_rotation_function, rotationHandlerFunction ccw_rotation_function);

#endif
