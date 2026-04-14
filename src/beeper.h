#ifndef BEEPER_H
#define BEEPER_H

#include <Arduino.h>

class Beeper
{
private:
    uint8_t _pin;

public:
    // Конструктор принимает пин зуммера
    Beeper(uint8_t pin);

    // Инициализация пина
    void begin();

    // Основная функция писка
    // freq - частота в Гц
    // duration - длительность звучания в мс
    // pause - пауза между повторами в мс (по умолчанию 0)
    // count - количество повторов (по умолчанию 1)
    void beep(uint16_t freq, uint16_t duration, uint16_t pause = 0, uint8_t count = 1);
};

#endif