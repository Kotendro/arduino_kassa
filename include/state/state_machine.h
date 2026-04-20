#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "event.h"
#include "state/state_stack.h"
#include "card_uid.h"

class StateMachine
{
private:
    AppState currentState_;
    StateStack stateStack_;

    CardUID firstCard_;
    CardUID secondCard_;
    int inputValue_;
    char currentKey_;

    void enterState(AppState state);
    void resetContext();

    void runScenario1();
    void runScenario2();
    void runScenario3();

public:
    StateMachine();

    void setState(AppState newState, bool pushToStack = true);
    void goBack();

    void handleEvent(const Event& event);
    void loop();
};

#endif