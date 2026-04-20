#ifndef CARD_UID_H
#define CARD_UID_H

#include <Arduino.h>
#include <MFRC522.h>

struct CardUID
{
    byte size = 0;
    byte bytes[10] = {0};

    void clear();
    bool isEmpty() const;
    void printToSerial() const;
    void copyFrom(const MFRC522::Uid &uid);
};

#endif