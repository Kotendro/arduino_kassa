#pragma once

#include <etl/stack.h>
#include "types.h"
#include "components/number_input.h"

class StateMachine
{
private:
    State currentState_ = State::Idle;
    etl::stack<State, 10> prevStateStack_; 

    // Данные, которые мы собираем
    CardUID firstCard_;
    CardUID secondCard_;
    NumberInput input_;
    char currentKey_ = 0;

    void enterState(State state);
    void resetContext();

    void intoPrevState();

    void runScenario1();
    void runScenario2();
    void runScenario3();

public:
    StateMachine();

    void setState(State newState);

    void handleEvent(const Event& event); 

    State getCurrentState() const { return currentState_; }
};