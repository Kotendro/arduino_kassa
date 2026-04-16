#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "state/state_stack.h"

class StateMachine {
private:
    AppState currentState_;
    StateStack stateStack_;

    void enterState(AppState state);
    void updateState(AppState state);

public:
    StateMachine();

    void setState(AppState newState, bool pushToStack = true);
    void goBack();
    void loop();
};

#endif