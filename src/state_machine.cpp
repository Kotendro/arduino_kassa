#include "state_machine.h"
#include "debug_logger.h"

StateMachine::StateMachine(LCD_1602_RUS& lcd, Beeper& beeper)
: monitor_(lcd), beeper_(beeper) {}

void StateMachine::init() 
{
    enterState(currentState_);
    monitor_.renderTransactionScreen(topAccount_, bottomAccount_, input_, direction_);
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
        case State::SystemMessage:
        {
            timer_ = millis();
        }
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
                monitor_.renderSystemMessage("ДОСТИГНУТО МАКС.\nКОЛ-ВО ИГРОКОВ");
                setState(State::SystemMessage);
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
                if (bottomAccount_ == nullptr) {
                    direction_ = TransactionDirection::BottomToTop;
                }
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
                monitor_.renderSystemMessage("ДОСТИГНУТО МАКС.\nКОЛ-ВО ИГРОКОВ");
                setState(State::SystemMessage);
                return;
            }
            
            Account* newAcc = optAcc.value();

            if (topAccount_ == newAcc || bottomAccount_ == newAcc) {
                DEBUG_ERROR(F("The same card"));
                monitor_.renderSystemMessage("ДАННАЯ КАРТА УЖЕ\nСЧИТАНА");
                setState(State::SystemMessage);
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
            if (bottomAccount_ != nullptr)
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
                monitor_.renderSystemMessage("ДОСТИГНУТО МАКС.\nКОЛ-ВО ИГРОКОВ");
                setState(State::SystemMessage);
                return;
            }
            
            Account* newAcc = optAcc.value();

            if (topAccount_ == newAcc || bottomAccount_ == newAcc) {
                DEBUG_ERROR(F("The same card"));
                monitor_.renderSystemMessage("ДАННАЯ КАРТА УЖЕ\nСЧИТАНА");
                setState(State::SystemMessage);
                return;
            }

            pushAccount(newAcc);
            break;
        }
        default : {}
        }

        break;    
    }
    case State::SystemMessage: 
    {
        switch (event.type) {
            case EventType::Cancel :
            {
                intoPrevState();
                monitor_.renderTransactionScreen(topAccount_, bottomAccount_, input_, direction_);
                break;
            }
            default : { return; } // Не пропускаем к дальнейшей отрисовке
        }

        break;
    }
    }
    DEBUG_MONITOR_SERIAL(topAccount_, bottomAccount_, input_, direction_);
    monitor_.renderTransactionScreen(topAccount_, bottomAccount_, input_, direction_);
}

/*
* Логика, которая выполняется каждый тик.
*/
void StateMachine::update() {
    if (currentState_ == State::SystemMessage) {
        if (millis() - timer_ >= 2500) {
            intoPrevState();
            monitor_.renderTransactionScreen(topAccount_, bottomAccount_, input_, direction_);
        }
    }
}