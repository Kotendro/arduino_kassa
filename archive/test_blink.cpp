#include <Arduino.h>

void setup()
{
    const int leds[] = {3, 4, 5};
    for (int i = 0; i < 3; i++)
        pinMode(leds[i], OUTPUT);

    while (1)
    { // бесконечно мигаем
        for (int i = 0; i < 3; i++)
        {
            digitalWrite(leds[i], HIGH);
            delay(500);
            digitalWrite(leds[i], LOW);
        }
    }
}

void loop() {}