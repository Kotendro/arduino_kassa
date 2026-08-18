#pragma once

#include <Arduino.h>
#include <MFRC522.h>

struct Card : public MFRC522::Uid
{
    static constexpr uint8_t MAX_SiZE = 10;

    Card() = default;
    Card(const MFRC522::Uid& raw) : MFRC522::Uid(raw) {
        
    };

    void clear();
    void printToSerial() const;
    uint64_t packInto64() const;
};