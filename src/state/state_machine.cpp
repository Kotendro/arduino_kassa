#include "state/state_machine.h"

StateMachine::StateMachine()
    : currentState_(STATE_IDLE),
      inputValue_(0),
      currentKey_(0)
{
    enterState(currentState_);
}

void StateMachine::setState(AppState newState, bool pushToStack)
{
    if (pushToStack)
    {
        stateStack_.push(currentState_);
    }

    currentState_ = newState;
    enterState(currentState_);
}

void StateMachine::goBack()
{
    if (!stateStack_.isEmpty())
    {
        currentState_ = stateStack_.pop();
        enterState(currentState_);
    }
    else
    {
        currentState_ = STATE_IDLE;
        enterState(currentState_);
    }
}

void StateMachine::enterState(AppState state)
{
    switch (state)
    {
    case STATE_IDLE:
        resetContext();
        // показать "Ожидание карты или ввода"
        break;

    case STATE_INPUTTING:
        // Выводим первую нажатую клавишу
        Serial.print("Начало ввода. Первая клавиша: '");
        Serial.print(currentKey_);
        Serial.println("'");
        break;

    case STATE_AFTER_INPUT:
        // показать "Ожидание карты или отмены"
        break;

    case STATE_AFTER_CARD:
        // показать "Ожидание ввода, карты или отмены"
        break;

    case STATE_AFTER_SECOND_CARD:
        // показать "Ожидание ввода или отмены"
        break;
    }
}

void StateMachine::resetContext()
{
    firstCard_.clear();
    secondCard_.clear();

    inputValue_ = 0;
    currentKey_ = 0;
    stateStack_.reset();
}

void StateMachine::runScenario1()
{
    // Тут логика сценария 1:
    // сначала был ввод, потом карта
}

void StateMachine::runScenario2()
{
    // Тут логика сценария 2:
    // сначала карта, потом ввод
}

void StateMachine::runScenario3()
{
    // Тут логика сценария 3:
    // сначала карта, потом карта, потом ввод
}

void StateMachine::handleEvent(const Event &event)
{
    if (event.type == EVENT_NONE)
        return;

    switch (currentState_)
    {
    case STATE_IDLE:
    {
        if (event.type == EVENT_INPUT_READY)
        {
            inputValue_ = event.inputValue;
            setState(STATE_AFTER_INPUT, false);
        }
        else if (event.type == EVENT_CARD_READ)
        {
            firstCard_ = event.card;
            setState(STATE_AFTER_CARD, false);
        }
        else if (event.type == EVENT_KEY_PRESS) // <-- новое
        {
            currentKey_ = event.key;
            setState(STATE_INPUTTING, false);
        }
        break;
    }

    case STATE_AFTER_INPUT:
    {
        if (event.type == EVENT_CANCEL)
        {
            setState(STATE_IDLE, false);
        }
        else if (event.type == EVENT_CARD_READ)
        {
            firstCard_ = event.card;
            runScenario1();
            setState(STATE_IDLE, false);
        }
        // KEY_PRESS игнорируется
        break;
    }

    case STATE_AFTER_CARD:
    {
        if (event.type == EVENT_CANCEL)
        {
            setState(STATE_IDLE, false);
        }
        else if (event.type == EVENT_INPUT_READY)
        {
            inputValue_ = event.inputValue;
            runScenario2();
            setState(STATE_IDLE, false);
        }
        else if (event.type == EVENT_CARD_READ)
        {
            secondCard_ = event.card;
            setState(STATE_AFTER_SECOND_CARD, false);
        }
        else if (event.type == EVENT_KEY_PRESS) // <-- новое
        {
            currentKey_ = event.key;
            setState(STATE_INPUTTING, false);
        }
        break;
    }

    case STATE_AFTER_SECOND_CARD:
    {
        if (event.type == EVENT_CANCEL)
        {
            setState(STATE_IDLE, false);
        }
        else if (event.type == EVENT_INPUT_READY)
        {
            inputValue_ = event.inputValue;
            runScenario3();
            setState(STATE_IDLE, false);
        }
        else if (event.type == EVENT_KEY_PRESS) // <-- новое
        {
            currentKey_ = event.key;
            setState(STATE_INPUTTING, false);
        }
        break;
    }

    case STATE_INPUTTING: // <-- новое
    {
        if (event.type == EVENT_CANCEL)
        {
            goBack(); // возврат к предыдущему состоянию
        }
        // Остальные события пока игнорируем
        break;
    }
    }
}

void StateMachine::loop() {}