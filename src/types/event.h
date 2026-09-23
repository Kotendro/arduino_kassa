#pragma once

#include <Arduino.h>
#include <types/card.h>

enum class EventType : uint8_t
{
    None,
    CardRead,
    Cancel,
    Confirm,
    KeyPressed,
    SwitchDirection
};

struct Event
{
    EventType type = EventType::None;
    Card card;
    char key = 0;
};