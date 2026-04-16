// state_machine.cpp
#include "state_machine.h"
#include "Beeper.h"

// Внешние функции, определённые в main.cpp
extern Beeper buzzer;
extern void lcdPrint(const char* line1, const char* line2);
extern void lcdPrintCardUID(const String& uid);
extern bool isCancelPressed();   // проверяет 'C' или '0'
extern bool isCardPresent();
extern String getCardUID();

static StateStack stateStack;
static AppState currentState = STATE_STARTUP;

// ---- StateStack implementation ----
StateStack::StateStack() : depth(0) {}

void StateStack::push(AppState state) {
    if (depth < MAX_DEPTH) {
        stack[depth++] = state;
    }
}

AppState StateStack::pop() {
    if (depth > 0) {
        return stack[--depth];
    }
    return STATE_WAIT_FOR_CARD; // fallback
}

AppState StateStack::current() const {
    if (depth > 0) {
        return stack[depth - 1];
    }
    return STATE_WAIT_FOR_CARD;
}

void StateStack::reset() {
    depth = 0;
}

bool StateStack::isEmpty() const {
    return depth == 0;
}

// ---- State management ----
void setState(AppState newState, bool pushToStack) {
    if (pushToStack) {
        stateStack.push(currentState);
    }
    currentState = newState;
    
    // При входе в состояние обновляем экран
    switch (currentState) {
        case STATE_WAIT_FOR_CARD:
            lcdPrint("Жду карту", "абвгдеёжзилкмноп");
            break;
        case STATE_CARD_SHOWN:
            // экран будет обновлён отдельно при получении UID
            break;
        default:
            break;
    }
}

void goBack() {
    if (!stateStack.isEmpty()) {
        currentState = stateStack.pop();
    } else {
        currentState = STATE_WAIT_FOR_CARD;
    }
    // Обновим экран при возврате
    if (currentState == STATE_WAIT_FOR_CARD) {
        lcdPrint("Снова жду карту", "");
    }
}

// ---- Main state machine loop ----
void stateMachineLoop() {
    switch (currentState) {
        case STATE_STARTUP:
            setState(STATE_WAIT_FOR_CARD, false);
            break;
            
        case STATE_WAIT_FOR_CARD:
            if (isCardPresent()) {
                String uid = getCardUID();
                buzzer.beep(2000, 50, 200, 1);
                lcdPrintCardUID(uid);
                setState(STATE_CARD_SHOWN);
            }
            break;
            
        case STATE_CARD_SHOWN:
            if (isCancelPressed()) {
                buzzer.beep(1500, 30, 100, 1);
                goBack();
            }
            break;
    }
}