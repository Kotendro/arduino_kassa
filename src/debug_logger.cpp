#include "debug_logger.h"

/* 
* DEBUG
* Превращает enum State в строку.
*/
static const char* stateToString(State state)
{
    switch (state)
    {
        case State::Idle : return "IDLE";
        case State::AfterInput : return "AFTER_INPUT";
        case State::AfterCard : return "AFTER_CARD";
        case State::AfterSecondCard : return "AFTER_SECOND_CARD";
        case State::Inputting : return "INPUTTING";
        default: return "UNKNOWN";
    }
}

/* 
* DEBUG
* Превращает enum Event в строку.
*/
static const char* eventTypeToString(EventType event)
{
    switch (event)
    {
        case EventType::None : return "NONE";
        case EventType::CardRead : return "CARD_READ";
        case EventType::Cancel : return "CANCEL";
        case EventType::Confirm : return "CONFIRM";
        case EventType::KeyPressed : return "KEY_PRESS";
        case EventType::SwitchDirection : return "SWITCH_DIRECTION";
        default: return "UNKNOWN";
    }
}

void debugCard(const Card& card) {
    Serial.print(F("Card: "));
    card.printToSerial();
    Serial.println();
}

void debugEvent(const Event& event) {
    Serial.print(F("Event Type: "));
    Serial.println(eventTypeToString(event.type));
    Serial.println();
}

void debugState(const State& state) {
    Serial.print(F("Entering state: "));
    Serial.println(stateToString(state));
    Serial.println();
}

void debugError(const __FlashStringHelper* msg)
{
    Serial.print(F("ERROR: "));
    Serial.println(msg);
    Serial.println();
}