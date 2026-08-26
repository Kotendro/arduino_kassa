#include "config.h"

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <I2CKeyPad.h>
#include <LCD_1602_RUS_ALL.h>

#include "types.h"
#include "components.h"
#include "state_machine.h"

MFRC522 rfid(SS_PIN, RST_PIN);

Keypad keyPad(KEYPAD_ADDRESS);
char keys[] = "C987<654^321=,0+NF";

Beeper beeper(BUZZER_PIN);

LCD_1602_RUS lcd(LCD_ADDRESS, 16, 2);

StateMachine stateMachine(lcd, beeper);

Event pullEvent()
{
    Event event;

    // KeyPad
    keyPad.update();

    uint8_t idx = keyPad.getKeyDown();
    if (idx != I2C_KEYPAD_NOKEY)
    {
        char key = keys[idx];

        switch (key)
        {
        case 'C':
            event.type = EventType::Cancel;
            break;
        case '=':
            event.type = EventType::Confirm;
            break;
        case '+':
            event.type = EventType::SwitchDirection;
            break;
        default:
            event.type = EventType::KeyPressed;
            event.key = key;
            break;
        }
        return event;
    }

    // RFID
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
    {
        beeper.beep(2000, 50);

        event.type = EventType::CardRead;
        event.card = rfid.uid;

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
        return event;
    }

    return event;
}

void setup()
{
    Serial.begin(9600);
    Serial.println(F("Started"));

    // RFID
    SPI.begin();
    rfid.PCD_Init();

    // Beeper
    beeper.begin();
    beeper.beep(2000, 50);

    // Wire
    Wire.begin();
    Wire.setClock(400000);

    // KeyPad
    keyPad.setKeyPadMode(I2C_KEYPAD_4x4);
    if (keyPad.begin() == false)
    {
        Serial.println(F("\nERROR: cannot communicate to keypad.\nPlease reboot.\n"));
        while(1);
    }

    // LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);
}

void loop()
{
    Event event = pullEvent();
    stateMachine.handleEvent(event);
}
