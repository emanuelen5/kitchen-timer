#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <stdint.h>
#include <button.h>

#define CH_A_PIN PD2 // INT0
#define CH_B_PIN PD3  // INT1
#define SW_PIN PD4  // PCINT0

void init_hw_rotary_encoder(Button &button_);
void service_button_press();

#endif // ROTARY_ENCODER_H
