#ifndef EVENT_H
#define EVENT_H

#include <Arduino.h>
#include <card_uid.h>

enum EventType : uint8_t
{
    EVENT_NONE,
    EVENT_CARD_READ,
    EVENT_INPUT_READY,
    EVENT_CANCEL,
    EVENT_CONFIRM,
    EVENT_KEY_PRESS,
};

struct Event
{
    EventType type = EVENT_NONE;
    CardUID card;
    int inputValue = 0;
    char key = 0;
};

#endif