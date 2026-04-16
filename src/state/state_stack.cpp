#include "state/state_stack.h"

StateStack::StateStack() : depth_(0) {}


void StateStack::push(AppState state) {
    if (depth_ < MAX_DEPTH_) {
        stack_[depth_++] = state;
    }
}


AppState StateStack::pop() {
    if (depth_ > 0) {
        return stack_[--depth_];
    }
    return STATE_WAIT_FOR_CARD; // fallback
}


AppState StateStack::current() const {
    if (depth_ > 0) {
        return stack_[depth_ - 1];
    }
    return STATE_WAIT_FOR_CARD;
}


void StateStack::reset() {
    depth_ = 0;
}


bool StateStack::isEmpty() const {
    return depth_ == 0;
}