#include <Arduino.h>
#include "led-counter.h"


enum states {
    START,
    LOOP,
    END,
} state;

enum events {
    START_LOOPING,
    PRINT_HELLO,
    STOP_LOOPING,
};

void step_state(enum events event);

void setup() {
init_led_counter();

}

void loop() {
    step_state(START_LOOPING);
    step_state(PRINT_HELLO);
    step_state(PRINT_HELLO);
    step_state(STOP_LOOPING);
}


void step_state(enum events event) {
    switch(state) {
    case START:
        switch(event) {
        case START_LOOPING:
            state = LOOP;
            break;
        default:
            break;
        }       
        break;
    case LOOP:
        switch(event) {
        case PRINT_HELLO:
            break;
        case STOP_LOOPING:
            state = END;
            break;
        default:
            break;
        }
        break;
    case END:
        state = START;
        break;
    }
    static uint8_t transition = 0;
    set_counter(state | (transition ? bit(2) : 0));
    transition = !transition;
    delay(500);
}
