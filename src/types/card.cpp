#include "types/card.h"

void Card::clear()
{
    size = 0;

    for (byte i = 0; i < MAX_SiZE; i++)
    {
        uidByte[i] = 0;
    }
}

/*
* DEBUG
*
* Вывод UID в консоль.
*/
void Card::printToSerial() const
{
    for (byte i = 0; i < MAX_SiZE; i++)
    {
        if (uidByte[i] < 0x10)
            Serial.print('0');

        Serial.print(uidByte[i], HEX);

        if (i < MAX_SiZE - 1)
            Serial.print(':');
    }

    Serial.println("");
}

uint64_t Card::packInto64() const 
{
    uint64_t packed = 0;
    for (uint8_t i = 0; i < 8; i++) {
        packed = (packed << 8) | uidByte[i];
    }
    return packed;
}