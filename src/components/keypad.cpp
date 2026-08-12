#include "keypad.h"

/*
* Обновляет текущее состояние клавиатуры (state_).
* Обновляет значение текущей нажатой/зажатой кнопки (currentKey_).
* Обновляет возможность получения значения (isKeyRead_).
*/
void Keypad::update() {
    uint8_t key = getKey();

    // NoKey
    if (key == I2C_KEYPAD_NOKEY || key >= 16) {
        state_ = KeyState::NoKey;
        currentKey_ = I2C_KEYPAD_NOKEY;
        isKeyRead_ = true;
        return;
    }

    // Down
    if (state_ == KeyState::NoKey) {
        state_ = KeyState::Down;
        currentKey_ = key;
        downAt_ = millis();
        isKeyRead_ = false;
        return;
    }

    // Hold
    if (state_ == KeyState::Down && (millis() - downAt_ > holdThreshold_)) {
        state_ = KeyState::Hold;
    }
}

/*
* Единоразово получаем index клавиши, когда нажимаем на нее.
*/
uint8_t Keypad::getKeyDown() {
    if (!isKeyRead_) {
        isKeyRead_ = true;
        return currentKey_;
    }
    return I2C_KEYPAD_NOKEY;
}

KeyState Keypad::getKeyState() {
    return state_;
}