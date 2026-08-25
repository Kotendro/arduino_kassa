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
    direction_ = TransactionDirection::BottomToTop;

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

    switch (currentState_) {
    case State::Idle:
    {
        switch (event.type) {
        case EventType::CardRead :
        {
            DEBUG_CARD(event.card);

            const etl::optional<Account*> optAcc = bank_.getOrCreateAcc(event.card);
            if (!optAcc.has_value()) {
                DEBUG_ERROR(F("Max limit")); 
                return;
            }

            pushAccount(optAcc.value());
            setState(State::Inputting);
            break;
        }
        case EventType::KeyPressed :
        {
            currentKey_ = event.key;
            setState(State::Inputting);
            break;
        }
        case EventType::SwitchDirection :
        {
            direction_ = toggleDirection(direction_);
            break;
        }
        default : {}
        }

        break;
    }

    case State::Inputting:
    {
        switch (event.type) {
        case EventType::Cancel :
        {
            if (!input_.isEmpty()) {
                input_.clear();
            } else if (topAccount_ != &cbAccount_) {
                popAccount();
            } else {
                intoPrevState();
            }
            break;
        }    
        case EventType::CardRead :
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
            break;
        }
        case EventType::Confirm :
        {
            if (input_.isEmpty()) return;
            bank_.runTransaction(topAccount_, bottomAccount_, input_, direction_);
            setState(State::AfterTransaction);
            break;
        }
        case EventType::SwitchDirection :
        {
            direction_ = toggleDirection(direction_);
            break;
        }
        case EventType::KeyPressed :
        {
            currentKey_ = event.key;
            input_.enterKey(currentKey_);
            break;
        }
        default : {}
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
        switch (event.type) {
        case EventType::Confirm :
        {
            bank_.runTransaction(topAccount_, bottomAccount_, input_, direction_);
            break;
        }
        case EventType::Cancel :
        {
            setState(State::Idle);
            break;
        }
        case EventType::SwitchDirection :
        {
            direction_ = toggleDirection(direction_);
            break;
        }
        case EventType::CardRead :
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
            break;
        }
        default : {}
        }

        break;    
    }
    }
    DEBUG_MONITOR_SERIAL(topAccount_, bottomAccount_, input_, direction_);    
}