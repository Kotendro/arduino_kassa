#pragma once

#include <Arduino.h>
#include <types/card_uid.h>

enum class EventType : uint8_t
{
    None,
    CardRead,
    Cancel,
    Confirm,
    KeyPressed
};

struct Event
{
    EventType type = EventType::None;
    CardUID card;
    char key = 0;
};