#include "Beeper.h"

Beeper::Beeper(uint8_t pin) {
    _pin = pin;
}

void Beeper::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void Beeper::beep(uint16_t freq, uint16_t duration, uint16_t pause, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        tone(_pin, freq);
        delay(duration);
        noTone(_pin);
        if (i < count - 1) {
            delay(pause);
        }
    }
}