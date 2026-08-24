#pragma once

#include "config.h"
#include "components.h"
#include "types.h"

void debugCard(const Card& card);
void debugEvent(const Event& event);
void debugState(const State& state);
void debugError(const __FlashStringHelper* msg);
void debugMonitorSerial(const Account* topAcc, const Account* bottomAcc, TransactionDirection dir);

#ifdef DEBUG_MODE

#define DEBUG_CARD(card)        debugCard(card)
#define DEBUG_EVENT(event)      debugEvent(event)
#define DEBUG_STATE(state)      debugState(state)
#define DEBUG_ERROR(msg)        debugError(msg)
#define DEBUG_MONITOR_SERIAL(topAcc, bottomAcc, dir) \
    debugMonitorSerial(topAcc, bottomAcc, dir)

#else

#define DEBUG_CARD(card)
#define DEBUG_EVENT(event)
#define DEBUG_STATE(state)
#define DEBUG_ERROR(msg)
#define DEBUG_MONITOR_SERIAL(topAcc, bottomAcc, dir)

#endif