// Программа отладки клавиатуры 4х4, подключенной через схему PCF8574AN.

#include "Wire.h"
#include "I2CKeyPad.h"
#include <Beeper.h>

#define BUZZER_PIN 6

const uint8_t KEYPAD_ADDRESS = 0x38;
I2CKeyPad keyPad(KEYPAD_ADDRESS);
Beeper beeper(BUZZER_PIN);

void setup()
{
    Serial.begin(9600);
    delay(500); // дадим время монитору порта
    Serial.println(F("=== Starting Keypad Test ==="));

    // beeper
    beeper.begin();
    beeper.beep(2000, 50);

    Wire.begin();
    Wire.setClock(400000);

    bool connected = false;

    for (int attempt = 1; attempt <= 3; attempt++)
    {
        Serial.print(F("Attempt "));
        Serial.print(attempt);
        Serial.print(F(": "));

        if (keyPad.begin())
        {
            Serial.println(F("SUCCESS"));
            connected = true;
            break;
        }
        else
        {
            Serial.println(F("FAILED"));
        }
        delay(500);
    }

    if (!connected)
    {
        Serial.println(F("ERROR: Could not connect to keypad after 3 attempts."));
        Serial.println(F("Check wiring and address."));
    }
    else
    {
        Serial.println(F("Keypad ready. Press keys..."));
    }
}

void loop()
{
    // Даже если не подключились — не зависаем
    if (keyPad.isConnected())
    {
        char keys[] = "C987<654^321=,0+NF";
        uint8_t idx = keyPad.getKey();
        if (idx != 255 && keys[idx] != 'N')
        {
            Serial.println(keys[idx]);
        }
    }
    else
    {
        // Просто мигаем встроенным светодиодом для индикации ошибки
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(500);
    }
}