#pragma once

#include <Arduino.h>

class Beeper
{
private:
    uint8_t pin_;

public:
    Beeper(uint8_t pin);

    /* Инициализация пина */
    void begin();

    /*
    * Основная функция писка
    * @param freq частота в Гц
    * @param duration длительность звучания в мс
    * @param pause пауза между повторами в мс (по умолчанию 0)
    * @param count количество повторов (по умолчанию 1)
    */
    void beep(uint16_t freq, uint16_t duration, uint16_t pause = 0, uint8_t count = 1);
};