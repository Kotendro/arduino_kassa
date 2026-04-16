#ifndef STATE_STACK_H
#define STATE_STACK_H

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
    static const uint8_t MAX_DEPTH_ = 8;
    AppState stack_[MAX_DEPTH_];
    uint8_t depth_;
public:
    StateStack();
    void push(AppState state);
    AppState pop();
    AppState current() const;
    void reset();
    bool isEmpty() const;
};

#endif