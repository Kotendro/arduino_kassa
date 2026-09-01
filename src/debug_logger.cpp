#include "debug_logger.h"

#ifdef DEBUG_MODE

/* 
* Превращает enum State в строку.
*/
static const char* stateToString(State state)
{
    switch (state)
    {
        case State::Idle : return "IDLE";
        case State::Inputting : return "INPUTTING";
        case State::AfterTransaction  : return "AFTER TRANSACTION";
        case State::SystemMessage : return "SYSTEM MESSAGE";
        default: return "UNKNOWN";
    }
}

/* 
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

void debugError(const char* msg)
{
    Serial.print(F("ERROR: "));
    Serial.println(msg);
    Serial.println();
}

void debugMonitorSerial(const Account* topAcc, const Account* bottomAcc, const NumberInput& input, TransactionDirection dir)
{
    Serial.println(F("================"));

    constexpr size_t bufferSize = 24;
    char buffer[bufferSize]; 

    // Top line
    if (topAcc == nullptr) {
        Serial.println();
    } else {
        bool isSender = (dir == TransactionDirection::TopToBottom);
        
        if (isSender) Serial.print(F("  "));
        else Serial.print(F("->"));
        
        if (topAcc->type == AccountType::CentralBank) {
            Serial.print(F("ЦБ :"));

            if (!isSender && !input.isEmpty()) {
                formatInput(buffer, bufferSize, input.getNumberInputView());
                Serial.println(buffer);
            } else {
                Serial.println(F("Безлимит ")); 
            }
        } else {
            Serial.print(F("И"));
            Serial.print(topAcc->id);
            Serial.print(F(" :"));

            if (!isSender && !input.isEmpty()) 
                formatInput(buffer, bufferSize, input.getNumberInputView());
            else
                formatBalance(buffer, bufferSize, topAcc);

            Serial.println(buffer);
        }
    }

    // Bottom line
    if (bottomAcc == nullptr) {
        Serial.println();
    } else {
        bool isSender = (dir == TransactionDirection::BottomToTop);
        
        if (isSender) Serial.print(F("  "));
        else Serial.print(F("->"));
        
        if (bottomAcc->type == AccountType::CentralBank) {
            Serial.print(F("ЦБ :"));
            if (!isSender && !input.isEmpty()) {
                formatInput(buffer, 16, input.getNumberInputView());
                Serial.println(buffer);
            } else {
                Serial.println(F("Безлимит "));
            }
        } else {
            Serial.print(F("И"));
            Serial.print(bottomAcc->id);
            Serial.print(F(" :"));
            if (!isSender && !input.isEmpty()) {
                formatInput(buffer, 16, input.getNumberInputView());
                Serial.println(buffer);
            } else {
                formatBalance(buffer, bufferSize, bottomAcc);
                Serial.println(buffer);
            }
        }
    }
    
    Serial.println(F("================"));
}

#endif