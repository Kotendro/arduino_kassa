#include <Arduino.h>
#include <Wire.h>

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    Serial.println(F("\n=== I2C Scanner with Address Logging ==="));

    pinMode(A4, INPUT_PULLUP);
    pinMode(A5, INPUT_PULLUP);

    Wire.begin();
    Wire.setWireTimeout(5000, true);
    Serial.println(F("Wire initialized. Scanning...\n"));
}

void loop()
{
    byte error, address;
    int nDevices = 0;

    for (address = 1; address < 127; address++)
    {
        // Выводим проверяемый адрес
        Serial.print(F("Scanning 0x"));
        if (address < 16)
            Serial.print('0');
        Serial.print(address, HEX);
        Serial.print(F("... "));

        Wire.beginTransmission(address);
        error = Wire.endTransmission(true);

        if (error == 0)
        {
            Serial.println(F("FOUND!"));
            nDevices++;
        }
        else if (error == 5)
        {
            Serial.println(F("TIMEOUT!"));
        }
        else
        {
            Serial.println(F("no response"));
        }
        delay(5);
    }

    Serial.println(F("\n--- Scan Complete ---"));
    if (nDevices == 0)
    {
        Serial.println(F("No devices found. Check pull-up resistors."));
    }
    else
    {
        Serial.print(F("Found "));
        Serial.print(nDevices);
        Serial.println(F(" device(s)."));
    }
    Serial.println(F("Restarting in 5 seconds...\n"));
    delay(5000);
}