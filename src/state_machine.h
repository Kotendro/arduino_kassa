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

    // Данные, которые мы собираем
    Card firstCard_;
    Card secondCard_;
    NumberInput input_;
    char currentKey_ = 0;

    // Компоненты 
    LCD_1602_RUS& lcd_;
    Beeper& beeper_;
    Bank bank_;

    void setState(State newState);
    void enterState(State state);

    void resetContext();

    void intoPrevState();

public:
    StateMachine(LCD_1602_RUS& lcd, Beeper& beeper);

    void handleEvent(const Event& event); 

    State getCurrentState() const { return currentState_; }
};