#pragma once

#include "config.h"
#include "components.h"
#include "types.h"

void debugCard(const Card& card);
void debugEvent(const Event& event);
void debugState(const State& state);
void debugError(const __FlashStringHelper* msg);

#ifdef DEBUG_MODE

#define DEBUG_CARD(card)        debugCard(card)
#define DEBUG_EVENT(event)      debugEvent(event)
#define DEBUG_STATE(state)      debugState(state)
#define DEBUG_ERROR(msg)        debugError(msg)

#else

#define DEBUG_CARD(card)
#define DEBUG_EVENT(event)
#define DEBUG_STATE(state)
#define DEBUG_ERROR(msg)

#endif