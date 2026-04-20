#include "card_uid.h"

void CardUID::clear()
{
    size = 0;

    for (byte i = 0; i < 10; i++)
    {
        bytes[i] = 0;
    }
}

bool CardUID::isEmpty() const
{
    return size == 0;
}

void CardUID::printToSerial() const
{
    for (byte i = 0; i < size; i++)
    {
        if (bytes[i] < 0x10)
            Serial.print("0");

        Serial.print(bytes[i], HEX);

        if (i < size - 1)
            Serial.print(":");
    }

    Serial.println("");
}

void CardUID::copyFrom(const MFRC522::Uid &uid)
{
    size = uid.size;

    for (byte i = 0; i < uid.size; i++)
    {
        bytes[i] = uid.uidByte[i];
    }

    for (byte i = uid.size; i < 10; i++)
    {
        bytes[i] = 0;
    }
}