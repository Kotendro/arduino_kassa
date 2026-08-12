#include "state_machine.h"
#include "types.h"

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
        default: return "UNKNOWN";
    }
}

StateMachine::StateMachine()
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
    Serial.print("Entering state: ");
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
            // Ввод символа
            if (
                (currentKey_ >= '0' && currentKey_ <= '9') ||
                (currentKey_ == ',')
            )
            {
                input_.addChar(currentKey_);
            }
            // Удаление символа
            else if (currentKey_ == '<')
            {
                input_.delChar();
            }
            // Возведение в степень
            else if (currentKey_ == '^')
            {
                input_.nextKiloPower();
            }

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

void StateMachine::runScenario1()
{
    // Тут логика сценария 1:
    // сначала был ввод, потом карта
    Serial.println("Scenario 1");
}

void StateMachine::runScenario2()
{
    // Тут логика сценария 2:
    // сначала карта, потом ввод
    Serial.println("Scenario 2");
}

void StateMachine::runScenario3()
{
    // Тут логика сценария 3:
    // сначала карта, потом карта, потом ввод
    Serial.println("Scenario 3");
}

void StateMachine::handleEvent(const Event &event)
{
    if (event.type == EventType::None)
        return;

    Serial.print("Event Type: ");
    Serial.println(eventTypeToString(event.type));

    switch (currentState_)
    {
        case State::Idle:
        {
            if (event.type == EventType::CardRead)
            {
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
                firstCard_ = event.card;
                runScenario1();
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
            else if (event.type == EventType::Confirm)
            {
                if (input_.isEmpty())
                    return;

                const State prevState = prevStateStack_.top();
                if (prevState == State::AfterCard)
                {
                    runScenario2();
                    setState(State::Idle);
                }
                else if (prevState == State::AfterSecondCard)
                {
                    runScenario3();
                    setState(State::Idle);
                }
                else if (prevState == State::Idle)
                {
                    setState(State::AfterInput);
                }
            }
            else if (event.type == EventType::KeyPressed)
            {
                currentKey_ = event.key;

                // Ввод символа
                if (
                    (currentKey_ >= '0' && currentKey_ <= '9') ||
                    (currentKey_ == ',')
                )
                {
                    input_.addChar(currentKey_);

                }
                // Удаление символа
                else if (currentKey_ == '<')
                {
                    input_.delChar();
                }
                // Возведение в степень
                else if (currentKey_ == '^')
                {
                    input_.nextKiloPower();
                }

                input_.printToSerial();
            }
            break;
        }
    }
}

