# pragma once

#include <Arduino.h>

enum class State : uint8_t
{
    Idle,
    Inputting,
    AfterTransaction
};
