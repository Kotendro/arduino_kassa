#include "state_machine.h"
#include "debug_logger.h"

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
    DEBUG_STATE(state);

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

/*
* В зависимости от текущего состояния (currentState_)
* и от пришедшего события (event) 
* выполняем операцию.
*/
void StateMachine::handleEvent(const Event &event)
{
    if (event.type == EventType::None)
        return;

    DEBUG_EVENT(event);

    switch (currentState_)
    {
        case State::Idle:
        {
            if (event.type == EventType::CardRead)
            {
                DEBUG_CARD(event.card);

                const etl::optional<Player*> player = bank_.getOrCreateAcc(event.card);
                if (!player.has_value()) {
                    DEBUG_ERROR(F("Max player limit"));
                    return;
                }

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
                DEBUG_CARD(event.card);

                const etl::optional<Player*> player = bank_.getOrCreateAcc(event.card);
                if (!player.has_value()) {
                    DEBUG_ERROR(F("Max player limit"));
                    return;
                }

                firstCard_ = event.card;
                bank_.runOneSideTransaction(firstCard_, input_);
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
                DEBUG_CARD(event.card);

                if (firstCard_.packInto64() == event.card.packInto64()) {
                    DEBUG_ERROR(F("The same card"));
                    return;
                }

                const etl::optional<Player*> player = bank_.getOrCreateAcc(event.card);
                if (!player.has_value()) {
                    DEBUG_ERROR(F("Max player limit"));
                    return;
                }

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
                if (input_.isEmpty())
                {
                    intoPrevState();
                    return;
                }
                input_.clear();
            }
            else if (event.type == EventType::CardRead)
            {
                DEBUG_CARD(event.card);

                if (firstCard_.size == 0) {
                    const etl::optional<Player*> player = bank_.getOrCreateAcc(event.card);
                    if (!player.has_value()) {
                        DEBUG_ERROR(F("Max player limit"));
                        return;
                    }
                    firstCard_ = event.card;
                } 
                else if (secondCard_.size == 0) {
                    if (firstCard_.packInto64() != event.card.packInto64()) {
                        const etl::optional<Player*> player = bank_.getOrCreateAcc(event.card);
                        if (!player.has_value()) {
                            DEBUG_ERROR(F("Max player limit"));
                            return;
                        }
                        secondCard_ = event.card;
                    } else {
                        DEBUG_ERROR(F("The same card"));
                    }
                }
            }
            else if (event.type == EventType::Confirm)
            {
                if (input_.isEmpty())
                    return;

                if (firstCard_.size > 0 && secondCard_.size > 0) 
                {
                    bank_.runTwoSideTransaction(firstCard_, secondCard_, input_);
                    setState(State::Idle);
                }
                else if (firstCard_.size > 0) 
                {
                    bank_.runOneSideTransaction(firstCard_, input_);
                    setState(State::Idle);
                }
                else {
                    setState(State::AfterInput);
                }
            }
            else if (event.type == EventType::SwitchDirection)
            {
                input_.switchDirection();
            }
            else if (event.type == EventType::KeyPressed)
            {
                currentKey_ = event.key;
                input_.enterKey(currentKey_);
            }
            break;
        }
    }
}
