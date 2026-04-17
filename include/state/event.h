#ifndef EVENT_H
#define EVENT_H

#include <Arduino.h>

enum EventType : uint8_t
{
    EVENT_NONE,
    EVENT_CARD_READ,
    EVENT_INPUT_READY,
    EVENT_CANCEL
};

struct Event
{
    EventType type = EVENT_NONE;
    uint32_t cardId = 0;
    int inputValue = 0;
};

#endif