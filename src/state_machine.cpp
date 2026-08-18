#include "state_machine.h"

/* 
* DEBUG
*
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
*
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

StateMachine::StateMachine(LCD_1602_RUS& lcd, Beeper& beeper)
: lcd_(lcd), beeper_(beeper)
{
    enterState(currentState_);
}


/* 
* Выполняет переход в новое состояние с сохранением истории.
* Текущее состояние помещается в стек (prevStateStack_), что позволяет 
* в дальнейшем вернуться назад.
*/
void StateMachine::setState(State newState)
{
    prevStateStack_.push(currentState_);

    currentState_ = newState;
    enterState(currentState_);
}

/* 
* Откат в предыдущее состояние.
*/
void StateMachine::intoPrevState()
{
    if (!prevStateStack_.empty())
    {
        prevStateStack_.pop_into(currentState_);
        enterState(currentState_);
    }
    else
    {
        currentState_ = State::Idle;
        enterState(currentState_);
    }
}

/* 
* Единоразовое действие при входе в состояние.
*/
void StateMachine::enterState(State state)
{
    Serial.print(F("Entering state: "));
    Serial.println(stateToString(state));

    switch (state)
    {
        case State::Idle:
        {
            resetContext();
            // показать "Ожидание карты или ввода"
            break;
        }
        case State::Inputting:  {
            input_.enterKey(currentKey_);

            input_.printToSerial();
            break;
        }
        case State::AfterInput:
            // показать "Ожидание карты или отмены"
            break;
        case State::AfterCard:
            // показать "Ожидание ввода, карты или отмены"
            break;
        case State::AfterSecondCard:
            // показать "Ожидание ввода или отмены"
            break;
    }
}

/* 
* Сброс собранных данных.
*/
void StateMachine::resetContext()
{
    firstCard_.clear();
    secondCard_.clear();

    input_.clear();
    currentKey_ = 0;
    prevStateStack_.clear();
}

void StateMachine::handleEvent(const Event &event)
{
    if (event.type == EventType::None)
        return;

    Serial.print(F("Event Type: "));
    Serial.println(eventTypeToString(event.type));

    switch (currentState_)
    {
        case State::Idle:
        {
            if (event.type == EventType::CardRead)
            {
                Serial.print(F("CardUID: "));
                event.card.printToSerial();

                const etl::optional<Player> player = bank_.getOrCreateAcc(event.card);
                if (!player.has_value()) {
                    // Сообщаем, что достигнут предел игроков
                    Serial.println(F("ERROR: Max player limit"));
                    return;
                }
                Serial.print(F("Player "));
                Serial.print(player.value().id);
                Serial.print(F(" balance: "));
                Serial.println((uint32_t)player.value().balance);

                firstCard_ = event.card;
                setState(State::AfterCard);
            }
            else if (event.type == EventType::KeyPressed)
            {
                currentKey_ = event.key;
                setState(State::Inputting);
            }
            break;
        }

        case State::AfterInput:
        {
            if (event.type == EventType::Cancel)
            {
                intoPrevState();
            }
            else if (event.type == EventType::CardRead)
            {
                Serial.print(F("CardUID: "));
                event.card.printToSerial();

                const etl::optional<Player> player = bank_.getOrCreateAcc(event.card);
                if (!player.has_value()) {
                    // Сообщаем, что достигнут предел игроков
                    Serial.println(F("ERROR: Max player limit"));
                    return;
                }
                Serial.print(F("Player "));
                Serial.print(player.value().id);
                Serial.print(F(" balance: "));
                Serial.println((uint32_t)player.value().balance);

                firstCard_ = event.card;
                bank_.runCashierTransaction(firstCard_, input_);
                setState(State::Idle);
            }
            break;
        }

        case State::AfterCard:
        {
            if (event.type == EventType::Cancel)
            {
                intoPrevState();
            }
            else if (event.type == EventType::CardRead)
            {
                Serial.print(F("CardUID: "));
                event.card.printToSerial();

                if (firstCard_.packInto64() == event.card.packInto64()) {
                    // Сообщаем, что одна и та же карта
                    Serial.println(F("ERROR: The same card"));
                    return;
                }

                const etl::optional<Player> player = bank_.getOrCreateAcc(event.card);
                if (!player.has_value()) {
                    // Сообщаем, что достигнут предел игроков
                    Serial.println(F("ERROR: Max player limit"));
                    return;
                }
                Serial.print(F("Player "));
                Serial.print(player.value().id);
                Serial.print(F(" balance: "));
                Serial.println((uint32_t)player.value().balance);

                secondCard_ = event.card;
                setState(State::AfterSecondCard);
            }
            else if (event.type == EventType::KeyPressed)
            {
                currentKey_ = event.key;
                setState(State::Inputting);
            }
            break;
        }
        
        case State::AfterSecondCard:
        {
            if (event.type == EventType::Cancel)
            {
                intoPrevState();
            }
            else if (event.type == EventType::KeyPressed)
            {
                currentKey_ = event.key;
                setState(State::Inputting);
            }
            break;
        }

        case State::Inputting:
        {
            if (event.type == EventType::Cancel)
            {
                // если экран не пустой, просто очищаем и не откатываемся
                if (input_.isEmpty())
                {
                    intoPrevState();
                    return;
                }
                input_.clear();
            }
            else if (event.type == EventType::CardRead)
            {
                Serial.print(F("CardUID: "));
                event.card.printToSerial();

                if (firstCard_.size == 0) {
                    firstCard_ = event.card;
                } 
                else if (secondCard_.size == 0) {
                    if (firstCard_.packInto64() != event.card.packInto64()) {
                        secondCard_ = event.card;
                    } else {
                        Serial.println(F("ERROR: The same card"));
                    }
                }
            }
            else if (event.type == EventType::Confirm)
            {
                if (input_.isEmpty())
                    return;

                if (firstCard_.size > 0 && secondCard_.size > 0) 
                {
                    bank_.runFPS();
                    setState(State::Idle);
                }
                else if (firstCard_.size > 0) 
                {
                    bank_.runCashierTransaction(firstCard_, input_);
                    setState(State::Idle);
                }
                else {
                    setState(State::AfterInput);
                }
            }
            else if (event.type == EventType::SwitchDirection)
            {
                input_.switchDirection();
                input_.printToSerial();
            }
            else if (event.type == EventType::KeyPressed)
            {
                currentKey_ = event.key;
                input_.enterKey(currentKey_);
                input_.printToSerial();
            }
            break;
        }
    }
}
