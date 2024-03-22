#include <Arduino.h>

void setup() {
    DDRB |= bit(0);
    PORTB |= bit(0);
}

void loop() {
    PORTB ^= bit(0);
    delay(1000);
}
