#ifndef NUMBER_INPUT_H
#define NUMBER_INPUT_H

#include <Arduino.h>

struct NumberInput
// Структура хранения введенного числа
{
    static constexpr uint8_t MAX_LEN = 16;

    char chars_[MAX_LEN] = {'\0'};
    uint8_t len_ = 0;
    bool comma_ = false;
    uint8_t kiloPower_ = 0; // 0,1,2 =>*10^0,*10^3,*10^6

    void clear();
    bool isEmpty() const;
    bool addChar(char c); // Добавляем символ, запятую 
    void nextKiloPower(); // 0 -> 1 -> 2 -> 0 -> ...
    void delChar(); // Удаляем символ, запятую
    void printToSerial() const;
};


#endif