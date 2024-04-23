#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#define CLK_PIN PD2 // INT0
#define DT_PIN PD3  // INT1
#define SW_PIN PD4

#define CW_INTERRUPT 1
#define CCW_INTERRUPT 2
#define QUEUE_SIZE 10

//Funtion pointer declaration
typedef void (*event_cb_t)(void);

struct Interrupt {
    int type;
};

void init_rotary_encoder(event_cb_t cw_rotation_cb, event_cb_t ccw_rotation_cb);
void queuing_interrupt(int interrupt);
void processInterrupt(struct Interrupt interrupt);
void dequeuing_interrupt();

#endif
