#include "state/state_machine.h"

static const char *stateToString(AppState state)
{
    switch (state)
    {
    case STATE_IDLE:
        return "IDLE";
    case STATE_AFTER_INPUT:
        return "AFTER_INPUT";
    case STATE_AFTER_CARD:
        return "AFTER_CARD";
    case STATE_AFTER_SECOND_CARD:
        return "AFTER_SECOND_CARD";
    case STATE_INPUTTING:
        return "INPUTTING";
    default:
        return "UNKNOWN";
    }
}

static const char *eventTypeToString(EventType event)
{
    switch (event)
    {
    case EVENT_NONE:
        return "NONE";
    case EVENT_CARD_READ:
        return "CARD_READ";
    case EVENT_CANCEL:
        return "CANCEL";
    case EVENT_CONFIRM:
        return "CONFIRM";
    case EVENT_KEY_PRESS:
        return "KEY_PRESS";
    default:
        return "UNKNOWN";
    }
}

StateMachine::StateMachine()
    : currentState_(STATE_IDLE),
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

void StateMachine::goBackNoEnter()
{
    if (!stateStack_.isEmpty())
    {
        currentState_ = stateStack_.pop();
    }
    else
    {
        currentState_ = STATE_IDLE;
    }
}

void StateMachine::enterState(AppState state)
{
    Serial.print("Entering state: ");
    Serial.println(stateToString(state));

    switch (state)
    {
    case STATE_IDLE:
    {
        resetContext();
        // показать "Ожидание карты или ввода"
        break;
    }

    case STATE_INPUTTING:
    {
        // Ввод символа
        if (
            (currentKey_ >= '0' && currentKey_ <= '9') ||
            (currentKey_ == ',')
        )
        {
            input_.addChar(currentKey_);
        }
        // Удаление символа
        else if (currentKey_ == '<')
        {
            input_.delChar();
        }
        // Возведение в степень
        else if (currentKey_ == '^')
        {
            input_.nextKiloPower();
        }

        input_.printToSerial();
        break;
    }

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

    input_.clear();
    currentKey_ = 0;
    stateStack_.reset();
}

void StateMachine::runScenario1()
{
    // Тут логика сценария 1:
    // сначала был ввод, потом карта
    Serial.println("Scenario 1");
}

void StateMachine::runScenario2()
{
    // Тут логика сценария 2:
    // сначала карта, потом ввод
    Serial.println("Scenario 2");
}

void StateMachine::runScenario3()
{
    // Тут логика сценария 3:
    // сначала карта, потом карта, потом ввод
    Serial.println("Scenario 3");
}

void StateMachine::handleEvent(const Event &event)
{
    if (event.type == EVENT_NONE)
        return;

    Serial.print("Event Type: ");
    Serial.println(eventTypeToString(event.type));

    switch (currentState_)
    {
    case STATE_IDLE:
    {
        if (event.type == EVENT_CARD_READ)
        {
            firstCard_ = event.card;
            setState(STATE_AFTER_CARD, true);
        }
        else if (event.type == EVENT_KEY_PRESS)
        {
            currentKey_ = event.key;
            setState(STATE_INPUTTING, true);
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
        break;
    }

    case STATE_AFTER_CARD:
    {
        if (event.type == EVENT_CANCEL)
        {
            setState(STATE_IDLE, false);
        }
        else if (event.type == EVENT_CARD_READ)
        {
            secondCard_ = event.card;
            setState(STATE_AFTER_SECOND_CARD, true);
        }
        else if (event.type == EVENT_KEY_PRESS)
        {
            currentKey_ = event.key;
            setState(STATE_INPUTTING, true);
        }
        break;
    }

    case STATE_AFTER_SECOND_CARD:
    {
        if (event.type == EVENT_CANCEL)
        {
            setState(STATE_IDLE, false);
        }
        else if (event.type == EVENT_KEY_PRESS)
        {
            currentKey_ = event.key;
            setState(STATE_INPUTTING, true);
        }
        break;
    }

    case STATE_INPUTTING:
    {
        if (event.type == EVENT_CANCEL)
        {
            // если экран не пустой, просто очищаем и не откатываемся
            if (input_.isEmpty())
            {
                goBack();
                return;
            }
            input_.clear();
        }
        else if (event.type == EVENT_CONFIRM)
        {
            if (input_.isEmpty())
                return;

            goBackNoEnter();
            if (currentState_ == STATE_AFTER_CARD)
            {
                runScenario2();
                setState(STATE_IDLE, false);
            }
            else if (currentState_ == STATE_AFTER_SECOND_CARD)
            {
                runScenario3();
                setState(STATE_IDLE, false);
            }
            else if (currentState_ == STATE_IDLE)
            {
                setState(STATE_AFTER_INPUT, false);
            }
        }
        else if (event.type == EVENT_KEY_PRESS)
        {
            currentKey_ = event.key;

            // Ввод символа
            if (
                (currentKey_ >= '0' && currentKey_ <= '9') ||
                (currentKey_ == ',')
            )
            {
                input_.addChar(currentKey_);

            }
            // Удаление символа
            else if (currentKey_ == '<')
            {
                input_.delChar();
            }
            // Возведение в степень
            else if (currentKey_ == '^')
            {
                input_.nextKiloPower();
            }

            input_.printToSerial();
        }
        break;
    }
    }
}

void StateMachine::loop() {}