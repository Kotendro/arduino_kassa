#pragma once

#include <I2CKeyPad.h>

enum class KeyState : uint8_t
{
    NoKey,
    Down,
    Hold,
};

/*
* Обертка над I2CKeyPad.
*
* Реализует функционал единоразового получения клавиши
* и получения текущего состояния (клавиша нажата, зажата и тп).
*/
class Keypad : public I2CKeyPad {
private:
    KeyState state_ = KeyState::NoKey;

    // Не можем получить значение нажатой кнопки более одного раза
    bool isKeyRead_; 

    uint8_t currentKey_;

    uint32_t downAt_;
    uint32_t holdThreshold_ = 600;


public:
    using I2CKeyPad::I2CKeyPad;

    void update();

    uint8_t getKeyDown();
    KeyState getKeyState();
};

