#include "state/state_stack.h"

StateStack::StateStack() : depth_(0) {}

void StateStack::push(AppState state)
{
    if (depth_ < MAX_DEPTH_)
    {
        stack_[depth_++] = state;
    }
}

AppState StateStack::pop()
{
    if (depth_ == 0)
    {
        return STATE_IDLE;
    }

    depth_--;
    return stack_[depth_];
}

AppState StateStack::current() const
{
    if (depth_ == 0)
    {
        return STATE_IDLE;
    }

    return stack_[depth_ - 1];
}

void StateStack::reset()
{
    depth_ = 0;
}

bool StateStack::isEmpty() const
{
    return depth_ == 0;
}