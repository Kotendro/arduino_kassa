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

/*
* Вспомогательная функция для форматирования баланса.
* Длина вывода всегда строго 9 символов (8 цифр/пробелов + 1 символ степени).
*/
static void printBalanceRightAligned(uint64_t val) {
    uint32_t displayVal;
    const char* power = " ";
    
    if (val >= 100000000ULL) {
        if (val >= 100000000000ULL) { // Больше 100 миллиардов
            displayVal = (uint32_t)(val / 1000000ULL);
            power = "м";
        } else {                      // От 100 млн до 100 млрд
            displayVal = (uint32_t)(val / 1000ULL);
            power = "т";
        }
    } else {
        displayVal = (uint32_t)val;
    }

    char buf[10];
    // %8lu автоматически добавит пробелы слева до 8 символов
    sprintf(buf, "%8lu", displayVal); 
    Serial.print(buf);
    Serial.print(power);
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

void debugMonitorSerial(const Account* topAcc, const Account* bottomAcc, const NumberInput& input, TransactionDirection dir)
{
    Serial.println(F("================")); // Ровно 16 символов экрана

    // --- ВЕРХНЯЯ СТРОКА ---
    if (topAcc == nullptr) {
        Serial.println();
    } else {
        bool isSender = (dir == TransactionDirection::TopToBottom);
        
        // Стрелочка указывает на получателя (не отправителя)
        if (isSender) Serial.print(F("  "));
        else Serial.print(F("->"));
        
        if (topAcc->type == AccountType::CentralBank) {
            Serial.print(F("ЦБ : "));
            // Если это получатель и ввод не пустой - показываем ввод
            if (!isSender && !input.isEmpty()) {
                input.printRightAlignedToSerial();
                Serial.println();
            } else {
                Serial.println(F("Безлимит ")); 
            }
        } else {
            Serial.print(F("И"));
            Serial.print(topAcc->id);
            Serial.print(F(" : "));
            if (!isSender && !input.isEmpty()) {
                input.printRightAlignedToSerial();
                Serial.println();
            } else {
                printBalanceRightAligned(topAcc->balance);
                Serial.println();
            }
        }
    }

    // --- НИЖНЯЯ СТРОКА ---
    if (bottomAcc == nullptr) {
        Serial.println();
    } else {
        bool isSender = (dir == TransactionDirection::BottomToTop);
        
        if (isSender) Serial.print(F("  "));
        else Serial.print(F("->"));
        
        if (bottomAcc->type == AccountType::CentralBank) {
            Serial.print(F("ЦБ : "));
            if (!isSender && !input.isEmpty()) {
                input.printRightAlignedToSerial();
                Serial.println();
            } else {
                Serial.println(F("Безлимит "));
            }
        } else {
            Serial.print(F("И"));
            Serial.print(bottomAcc->id);
            Serial.print(F(" : "));
            if (!isSender && !input.isEmpty()) {
                input.printRightAlignedToSerial();
                Serial.println();
            } else {
                printBalanceRightAligned(bottomAcc->balance);
                Serial.println();
            }
        }
    }
    
    Serial.println(F("================"));
}