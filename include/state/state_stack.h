#ifndef STATE_STACK_H
#define STATE_STACK_H

#include <Arduino.h>

enum AppState : uint8_t
{
    STATE_IDLE,
    STATE_INPUTTING,
    STATE_AFTER_INPUT,
    STATE_AFTER_CARD,
    STATE_AFTER_SECOND_CARD
};


class StateStack
{
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