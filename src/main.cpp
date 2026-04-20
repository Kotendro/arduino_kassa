#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Beeper.h>
#include <state/state_machine.h>

// Библиотеки для LCD и клавиатуры
#define _LCD_TYPE 1
#include <LCD_1602_RUS_ALL.h>
#include <OnewireKeypad.h>

// Пины и конфигурация
#define BUZZER_PIN 6
#define SS_PIN 10
#define RST_PIN 9
#define LCD_ADDR 0x27

// Клавиатура
char KEYS[] = {
    'C', '<', '^', '=',
    '9', '6', '3', ',',
    '8', '5', '2', '0',
    '7', '4', '1', '+'};

uint16_t ADC_THRESHOLDS[] = {
    767, 426, 299, 217,
    165, 141, 124, 106,
    92, 85, 78, 71,
    64, 60, 57, 53};

MFRC522 rfid(SS_PIN, RST_PIN);
StateMachine stateMachine;
OnewireKeypad<Print, 16> keypad(Serial, KEYS, ADC_THRESHOLDS, 4, 4, A1);
Beeper beeper(BUZZER_PIN);

Event pullEvent()
{
    Event event;

    // Если мы в режиме ввода с клавиатуры, RFID игнорируем
    if (stateMachine.getCurrentState() == STATE_INPUTTING)
    {
        char key = keypad.getkey();
        if (key != '\0')
        {
            if (key == 'C')
            {
                event.type = EVENT_CANCEL;
                Serial.println("Cancel key pressed");
                return event;
            }
            if ((key >= '0' && key <= '9') || key == ',' || key == '+')
            {
                event.type = EVENT_KEY_PRESS;
                event.key = key;
                switch (keypad.keyState())
                {
                case PRESSED:
                {
                    char key = keypad.getkey();
                    if (key != NO_KEY)
                    {
                        Serial << "Pressed: " << key << "\n";
                    }
                    break;
                }

                case HELD:
                {
                    char key = keypad.getkey();
                    if (key != NO_KEY)
                    {
                        Serial << "Held: " << key << "\n";
                    }
                    break;
                }

                case RELEASED:
                    Serial << "Released\n";
                    break;

                case WAITING:
                    break;
                }
                return event;
            }
            // Остальные клавиши ('<', '^', '=') пока игнорируем
        }
        return event; // EVENT_NONE
    }


    // RFID
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
    {
        event.type = EventType::EVENT_CARD_READ;
        event.card.copyFrom(rfid.uid);

        beeper.beep(2000, 50);
        event.card.printToSerial();

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
        return event;
    }

    // Клавиатура
    char key = keypad.getkey();
    if (key != '\0')
    {
        if (key == 'C')
        {
            event.type = EVENT_CANCEL;
            Serial.println("Отмена");
            return event;
        }
        // Цифры, запятая или плюс — начало ввода
        if ((key >= '0' && key <= '9') || key == ',' || key == '+')
        {
            event.type = EVENT_KEY_PRESS;
            event.key = key;
            switch (keypad.keyState())
            {
            case PRESSED:
            {
                char key = keypad.getkey();
                if (key != NO_KEY)
                {
                    Serial << "Pressed: " << key << "\n";
                }
                break;
            }

            case HELD:
            {
                char key = keypad.getkey();
                if (key != NO_KEY)
                {
                    Serial << "Held: " << key << "\n";
                }
                break;
            }

            case RELEASED:
                Serial << "Released\n";
                break;

            case WAITING:
                break;
            }
            return event;
        }
    }

    return event;
}

void setup()
{
    Serial.begin(9600);
    Serial.println("Started");

    // RFID
    SPI.begin();
    rfid.PCD_Init();

    // beeper
    beeper.begin();
    beeper.beep(2000, 50);
}

void loop()
{
    Event event = pullEvent();

    stateMachine.handleEvent(event);
}