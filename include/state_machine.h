// state_machine.h
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>

// Состояния автомата
enum AppState : uint8_t {
    STATE_STARTUP,
    STATE_WAIT_FOR_CARD,
    STATE_CARD_SHOWN
};

// Стек состояний
class StateStack {
private:
    static const uint8_t MAX_DEPTH = 8;
    AppState stack[MAX_DEPTH];
    uint8_t depth;
public:
    StateStack();
    void push(AppState state);
    AppState pop();
    AppState current() const;
    void reset();
    bool isEmpty() const;
};

void stateMachineLoop();
void setState(AppState newState, bool pushToStack = true);
void goBack();

#endif