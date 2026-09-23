#include "beeper.h"

Beeper::Beeper(uint8_t pin) : pin_(pin) {}

void Beeper::begin()
{
    pinMode(pin_, OUTPUT);
    digitalWrite(pin_, LOW);
}

void Beeper::beep(uint16_t freq, uint16_t duration, uint16_t pause, uint8_t count)
{
    for (uint8_t i = 0; i < count; i++)
    {
        tone(pin_, freq);
        delay(duration);
        noTone(pin_);
        if (i < count - 1)
        {
            delay(pause);
        }
    }
}