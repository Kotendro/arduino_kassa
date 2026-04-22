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
    char key = keypad.getkey();

    // Keyboard
    if (key != NO_KEY)
    {
        uint8_t state = keypad.keyState();
        if (state == PRESSED)
        {
            if (key >= '0' && key <= '9')
            {
                event.type = EVENT_KEY_PRESS;
                event.key = key;
                return event;
            }
        }
        else if (state == HELD)
        {
            if (key == 'C')
            {
                event.type = EVENT_CANCEL;
                return event;
            }
            else if (key == '=')
            {
                event.type = EVENT_CONFIRM;
                return event;
            }
        }
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