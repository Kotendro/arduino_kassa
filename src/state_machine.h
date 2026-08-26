#pragma once

#include "config.h"

#include <etl/stack.h>
#include <LCD_1602_RUS_ALL.h>
#include "components.h"
#include "types.h" 

class StateMachine
{
private:
    State currentState_ = State::Idle;
    etl::stack<State, 10> prevStateStack_; 

    Account cbAccount_ = {AccountType::CentralBank, 0, 0};
    Account* topAccount_ = &cbAccount_;
    Account* bottomAccount_ = nullptr; 
    
    TransactionDirection direction_ = TransactionDirection::BottomToTop;

    // Данные, которые мы собираем
    NumberInput input_;
    char currentKey_ = 0;

    // Компоненты 
    Monitor monitor_;
    Beeper& beeper_;
    Bank bank_;

    void setState(State newState);
    void enterState(State state);

    void resetContext();
    void intoPrevState();
    
    void pushAccount(Account* newAcc);
    void popAccount();

public:
    StateMachine(LCD_1602_RUS& lcd, Beeper& beeper);

    void handleEvent(const Event& event); 

    State getCurrentState() const { return currentState_; }
};