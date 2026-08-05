#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

#define _LCD_TYPE 1
#include <LCD_1602_RUS_ALL.h>

#include "types.h"
#include "state_machine.h"

// Пины и конфигурация
#define BUZZER_PIN 6
#define SS_PIN 10
#define RST_PIN 9
#define LCD_ADDR 0x27
#define KEYPAD_ADDRESS 0x38

MFRC522 rfid(SS_PIN, RST_PIN);
StateMachine stateMachine;
Beeper beeper(BUZZER_PIN);

Event pullEvent()
{
    Event event;

    // Клавиатура
    // Тут нужен код, пример в test_4x4_PCF8574.cpp

    // RFID
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
    {
        event.type = EventType::CardRead;
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
