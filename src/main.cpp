#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include "Beeper.h"


// Библиотеки для LCD и клавиатуры
#define _LCD_TYPE 1
#include <LCD_1602_RUS_ALL.h>
#include <OnewireKeypad.h>

// ---------- Пины и конфигурация ----------
#define BUZZER_PIN 6
#define SS_PIN 10
#define RST_PIN 9
#define LCD_ADDR 0x27


void setup()
{

}

void loop()
{

}