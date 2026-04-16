#include "state/state_machine.h"
#include <Arduino.h>

StateMachine::StateMachine()
    : currentState_(STATE_STARTUP) {
    enterState(currentState_);
}

// Первичное действие при смене состояния
void StateMachine::enterState(AppState state) {
    switch (state) {
        case STATE_STARTUP:
            Serial.println("ENTER: STARTUP");
            break;

        case STATE_WAIT_FOR_CARD:
            Serial.println("ENTER: WAIT_FOR_CARD");
            break;

        case STATE_CARD_SHOWN:
            Serial.println("ENTER: CARD_SHOWN");
            break;
    }
}


void StateMachine::updateState(AppState state) {
    switch (state) {
        case STATE_STARTUP:
            break;

        case STATE_WAIT_FOR_CARD:
            break;

        case STATE_CARD_SHOWN:
            break;
    }
}

void StateMachine::setState(AppState newState, bool pushToStack) {
    if (newState == currentState_) {
        return;
    }

    if (pushToStack) {
        stateStack_.push(currentState_);
    }

    currentState_ = newState;
    enterState(currentState_);
}

void StateMachine::goBack() {
    if (stateStack_.isEmpty()) {
        return;
    }

    currentState_ = stateStack_.pop();
    enterState(currentState_);
}

void StateMachine::loop() {
    updateState(currentState_);
}