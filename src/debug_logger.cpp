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
        case State::Inputting : return "INPUTTING";
        case State::AfterTransaction  : return "AFTER TRANSACTION";
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

void debugMonitorSerial(const Account* topAcc, const Account* bottomAcc, TransactionDirection dir)
{
    Serial.println(F("================"));

    // Top line
    if (topAcc == nullptr) {
        Serial.println();
    } else {
        if (dir == TransactionDirection::TopToBottom) Serial.print(F("  "));
        else Serial.print(F("->"));
        
        if (topAcc->type == AccountType::CentralBank) {
            Serial.println(F("ЦБ : Безлимит"));
        } else {
            Serial.print(F("И"));
            Serial.print(topAcc->id);
            Serial.print(F(" : "));
            Serial.println((uint32_t)topAcc->balance);
        }
    }

    // Bottom line
    if (bottomAcc == nullptr) {
        Serial.println();
    } else {
        if (dir == TransactionDirection::TopToBottom) Serial.print(F("->"));
        else Serial.print(F("  "));
        
        if (bottomAcc->type == AccountType::CentralBank) {
            Serial.println(F("ЦБ : Безлимит"));
        } else {
            Serial.print(F("И"));
            Serial.print(bottomAcc->id);
            Serial.print(F(" : "));
            Serial.println((uint32_t)bottomAcc->balance);
        }
    }
    
    Serial.println(F("================"));
}