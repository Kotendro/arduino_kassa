#pragma once

#include <Arduino.h>

/* 
* Cтруктура хранения введенного числа (без вывода на экран) 
*/
class NumberInput
{
private:
    static constexpr uint8_t MAX_LEN = 8; // max number - 9 999 9999 * M (10^6)

    char chars_[MAX_LEN] = {'\0'};
    uint8_t len_ = 0;
    bool comma_ = false;
    uint8_t kiloPower_ = 0;

    bool addChar(char c);
    void nextKiloPower();
    void delChar();

public:
    bool reverseDirection = false;

    bool isEmpty() const;

    void switchDirection();
    void enterKey(char key);

    uint64_t packInto64();

    void clear();
    
    void printToSerial() const;
};