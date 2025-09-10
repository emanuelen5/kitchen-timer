#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <stdint.h>
#include <button.h>

#define CH_A_PIN PD2 // INT0
#define CH_B_PIN PD3  // INT1
#define SW_PIN PD4  // PCINT0

typedef enum
{
    ccw,
    cw,
    none,
} rotation_dir_t;

typedef enum
{
    slow,
    fast,
} rotation_speed_t;

//Funtion pointer declaration
typedef void (*rotation_cb_t)(rotation_dir_t dir, rotation_speed_t speed, bool held_down);

void init_rotary_encoder(rotation_cb_t rotation_cb, Button &button_);
void service_button_press();

#endif // ROTARY_ENCODER_H
