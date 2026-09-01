#pragma once

#include <Arduino.h>

/*
* Данные из NumberInput, нужные для отображения вывода.
*/
struct NumberInputView {
    const char* chars_;
    uint8_t len_ = 0;
    uint8_t kiloPower_ = 0;
    uint8_t decimals_ = 0;
    bool comma_;
};

/* 
* Бизнес-логика ввода числа.
*/
class NumberInput
{
private:
    static constexpr uint8_t MAX_LEN = 9;

    char chars_[MAX_LEN] = {};
    uint8_t len_ = 0;
    uint8_t decimals_ = 0;
    bool comma_ = false;
    uint8_t kiloPower_ = 0;

    bool addChar(char c);
    void nextKiloPower();
    void delChar();

public:
    bool isEmpty() const;
    
    void enterKey(char key);
    void clear();

    uint64_t packInto64() const;
    NumberInputView getNumberInputView() const { 
        return {chars_, len_, kiloPower_, decimals_, comma_};
    }
};