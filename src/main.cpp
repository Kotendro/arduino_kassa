#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include "Beeper.h"

// Наши заголовки
#include "state_machine.h"

// Библиотеки для LCD и клавиатуры
#define _LCD_TYPE 1
#include <LCD_1602_RUS_ALL.h>
#include <OnewireKeypad.h>

// ---------- Пины и конфигурация ----------
#define BUZZER_PIN 6
#define SS_PIN 10
#define RST_PIN 9

// I2C адрес дисплея
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

// ---------- Глобальные объекты ----------
Beeper buzzer(BUZZER_PIN);
LCD_1602_RUS lcd(LCD_ADDR, 16, 2);
OnewireKeypad<Print, 16> keypad(Serial, KEYS, ADC_THRESHOLDS, 4, 4, A1);
MFRC522 rfid(SS_PIN, RST_PIN);

// ---------- Вспомогательные функции для state_machine ----------
void lcdPrint(const char *line1, const char *line2)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
}

void lcdPrintCardUID(const String &uid)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UID Карты:");
    lcd.setCursor(0, 1);
    lcd.print(uid);
}

bool isCancelPressed()
{
    // Проверяем, была ли нажата 'C' или '0' (как отмена)
    if (keypad.keyState() == PRESSED)
    {
        char key = keypad.getkey();
        return (key == 'C' || key == '0');
    }
    return false;
}

bool isCardPresent()
{
    // Быстрая проверка без задержек
    if (!rfid.PICC_IsNewCardPresent())
        return false;
    if (!rfid.PICC_ReadCardSerial())
        return false;
    return true;
}

String getCardUID()
{
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++)
    {
        if (rfid.uid.uidByte[i] < 0x10)
            uid += "0";
        uid += String(rfid.uid.uidByte[i], HEX);
        if (i < rfid.uid.size - 1)
            uid += ":";
    }
    uid.toUpperCase();

    // Завершаем работу с картой
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

    return uid;
}

// ---------- setup и loop ----------
void setup()
{
    Serial.begin(9600);

    // Пищалка
    buzzer.begin();
    buzzer.beep(1000, 100);

    // LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcdPrint("Тест 1", "абвгдеёжзилкмноп");
    delay(5000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Тест 2");
    lcd.setCursor(0, 1);
    lcd.print("абвгдеёжзилкмноп");
    delay(5000);

    // RFID
    SPI.begin();
    rfid.PCD_Init();

    // Клавиатура (не требует init)

    // Запускаем автомат
    setState(STATE_STARTUP, false);
}

void loop()
{
    // Обновляем состояние клавиатуры (библиотека сама опрашивает)
    // OnewireKeypad автоматически вызывает чтение при keyState()

    // Запускаем шаг конечного автомата
    stateMachineLoop();

    // Небольшая задержка для стабильности
    delay(20);
}