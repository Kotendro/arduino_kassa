#include <Arduino.h>
#include "Beeper.h"

#define BUZZER_PIN 6

Beeper buzzer(BUZZER_PIN);

void setup()
{
    Serial.begin(9600);

    // Инициализация зумера
    buzzer.begin();
    buzzer.beep(1000, 100);
}

void loop()
{
}