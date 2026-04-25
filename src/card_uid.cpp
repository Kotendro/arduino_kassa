#include "card_uid.h"

void CardUID::clear()
{
    size_ = 0;

    for (byte i = 0; i < MAX_LEN; i++)
    {
        bytes_[i] = 0;
    }
}

bool CardUID::isEmpty() const
{
    return size_ == 0;
}

void CardUID::printToSerial() const
{
    for (byte i = 0; i < size_; i++)
    {
        if (bytes_[i] < 0x10)
            Serial.print("0");

        Serial.print(bytes_[i], HEX);

        if (i < size_ - 1)
            Serial.print(":");
    }

    Serial.println("");
}

void CardUID::copyFrom(const MFRC522::Uid &uid)
{
    size_ = uid.size;

    for (byte i = 0; i < uid.size; i++)
    {
        bytes_[i] = uid.uidByte[i];
    }

    for (byte i = uid.size; i < MAX_LEN; i++)
    {
        bytes_[i] = 0;
    }
}