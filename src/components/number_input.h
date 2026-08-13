#pragma once

#include <Arduino.h>

/* 
* Cтруктура хранения введенного числа (без вывода на экран) 
*/
class NumberInput
{
private:
    static constexpr uint8_t MAX_LEN = 16;

    char chars_[MAX_LEN] = {'\0'};
    uint8_t len_ = 0;
    bool comma_ = false;
    uint8_t kiloPower_ = 0;
    bool reverseDirection = false;

    bool addChar(char c);
    void nextKiloPower();
    void switchDirection();
    void delChar();

public:
    bool isEmpty() const;
    void enterKey(char key);
    void clear();
    void printToSerial() const;
};