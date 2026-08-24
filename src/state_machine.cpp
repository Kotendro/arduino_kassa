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
            break;
        }
        case State::Inputting:  
        {
            input_.enterKey(currentKey_);
            break;
        }
        case State::AfterTransaction:
            break;
    }
}



void StateMachine::pushAccount(Account* newAcc)
{
    if (topAccount_ == &cbAccount_) {
        topAccount_ = newAcc;
        bottomAccount_ = &cbAccount_;
    } 
    else if (bottomAccount_ == &cbAccount_) {
        bottomAccount_ = newAcc;
    } 
    else {
        topAccount_ = bottomAccount_;
        bottomAccount_ = newAcc;
    }
}

void StateMachine::popAccount()
{
    if (bottomAccount_ != &cbAccount_ && bottomAccount_ != nullptr) 
    {
        bottomAccount_ = &cbAccount_;
    }
    else if (topAccount_ != &cbAccount_) 
    {
        topAccount_ = &cbAccount_;
        bottomAccount_ = nullptr;
    }
}

/* 
* Сброс собранных данных при переходе в Idle.
*/
void StateMachine::resetContext()
{
    topAccount_ = &cbAccount_;
    bottomAccount_ = nullptr;
    direction_ = TransactionDirection::TopToBottom;

    input_.clear();
    currentKey_ = 0;
    prevStateStack_.clear();
}

/*
* Основной обработчик событий
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

                const etl::optional<Account*> optAcc = bank_.getOrCreateAcc(event.card);
                if (!optAcc.has_value()) {
                    DEBUG_ERROR(F("Max limit")); 
                    return;
                }

                pushAccount(optAcc.value());
                setState(State::Inputting); // Сразу переходим в активный ввод/управление
            }
            else if (event.type == EventType::KeyPressed)
            {
                currentKey_ = event.key;
                setState(State::Inputting);
            }
            else if (event.type == EventType::SwitchDirection)
            {
                direction_ = toggleDirection(direction_);
            }
            break;
        }

        case State::Inputting:
        {
            if (event.type == EventType::Cancel)
            {
                if (!input_.isEmpty()) {
                    input_.clear();
                } else if (topAccount_ != &cbAccount_) {
                    popAccount();
                } else {
                    intoPrevState();
                }
            }
            else if (event.type == EventType::CardRead)
            {
                DEBUG_CARD(event.card);

                const etl::optional<Account*> optAcc = bank_.getOrCreateAcc(event.card);
                if (!optAcc.has_value()) { 
                    DEBUG_ERROR(F("Max limit"));
                    return;
                }
                
                Account* newAcc = optAcc.value();

                if (topAccount_ == newAcc || bottomAccount_ == newAcc) {
                    DEBUG_ERROR(F("The same card"));
                    return;
                }

                pushAccount(newAcc);
            }
            else if (event.type == EventType::Confirm)
            {
                if (input_.isEmpty()) return;
                bank_.runTransaction(topAccount_, bottomAccount_, input_, direction_);
                setState(State::AfterTransaction);
            }
            else if (event.type == EventType::SwitchDirection)
            {
                direction_ = toggleDirection(direction_);
            }
            else if (event.type == EventType::KeyPressed)
            {
                currentKey_ = event.key;
                input_.enterKey(currentKey_);
            }
            break;
        }

        /*
        * Можно в принципе тоже убрать данное состояний
        * но будет проблемой выходить из Inputting - надо
        * будет нажимать Cancel несколько раз.
        * Зато можно будет продолжать вводить и вставлять карты
        * Тут тоже можно реализовать данную логику, сделав например 
        * Cancel сразу возвращающим в отличии от Inputting.
        */
        case State::AfterTransaction:
        {
            if (event.type == EventType::Confirm)
            {
                bank_.runTransaction(topAccount_, bottomAccount_, input_, direction_);
            }
            else if (event.type == EventType::Cancel)
            {
                setState(State::Idle);
            }
            else if (event.type == EventType::SwitchDirection)
            {
                direction_ = toggleDirection(direction_);
            }
            break;    
        }
    }
    DEBUG_MONITOR_SERIAL(topAccount_, bottomAccount_, direction_);    
}