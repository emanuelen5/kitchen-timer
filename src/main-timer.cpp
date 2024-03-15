#include <Arduino.h>

void setup() {
    DDRB |= bit(1);
    PORTB |= bit(1);
}

void loop() {
    PORTB ^= bit(1);
    delay(1000);
}
