#pragma once

#include <Arduino.h>
#include <MFRC522.h>

/* 
* Структура хранения RFID карточки
*/
struct CardUID
{
    static constexpr uint8_t MAX_LEN = 10;

    byte size_ = 0;
    byte bytes_[MAX_LEN] = {0};

    void clear();
    bool isEmpty() const;
    void printToSerial() const;
    void copyFrom(const MFRC522::Uid &uid);
};