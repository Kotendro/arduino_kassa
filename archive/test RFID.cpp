#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Beeper.h>
#include <Wire.h> //  Подключаем библиотеку для работы с LCD дисплеем по шине I2C
// #include <LiquidCrystal_I2C.h>   //  Подключаем библиотеку для работы с шиной I2C
#define _LCD_TYPE 1
#include <LCD_1602_RUS_ALL.h>

#define BUZZER_PIN 6
#define RST_PIN 9
#define SS_PIN 10

#define LCD_ADDR 0x27

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;
// LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2); // 16 столбцов, 2 строки
LCD_1602_RUS lcd(LCD_ADDR, 16, 2);

Beeper buzzer(BUZZER_PIN);

// Переменные для отсчета времени
unsigned long startMillis = 0;
unsigned long seconds = 0;
unsigned long minutes = 0;
unsigned long hours = 0;

void printTimestamp()
{
  unsigned long currentMillis = millis();

  if (startMillis == 0)
  {
    startMillis = currentMillis;
  }

  unsigned long elapsed = currentMillis - startMillis;
  seconds = (elapsed / 1000) % 60;
  minutes = (elapsed / 60000) % 60;
  hours = (elapsed / 3600000);

  // Форматированный вывод времени [HH:MM:SS]
  if (hours < 10)
    Serial.print("0");
  Serial.print(hours);
  Serial.print(":");
  if (minutes < 10)
    Serial.print("0");
  Serial.print(minutes);
  Serial.print(":");
  if (seconds < 10)
    Serial.print("0");
  Serial.print(seconds);
  Serial.print(" - ");
}

void setup()
{
  Serial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();
  // Инициализация зумера
  buzzer.begin();
  buzzer.beep(1000, 100);
  // Инициализация дисплея
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("RFID Читалка");
  lcd.setCursor(0, 1);
  lcd.print("Жду карту...");

  printTimestamp();
  Serial.println("System started");

  // Диагностика модуля
  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.print("MFRC522 Version: 0x");
  Serial.println(version, HEX);

  if (version == 0x00 || version == 0xFF)
  {
    Serial.println("ERROR: MFRC522 not connected!");
    while (1)
      ;
  }

  // Ключ по умолчанию
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }

  Serial.println("Ready. Supported cards: MIFARE Classic, Ultralight, NTAG");
  Serial.println("Note: Bank cards, NFC phones may not be readable");
  Serial.println("================================================");
}

void loop()
{
  // Проверка на наличие новой карты
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Проверка на возможность чтения серийного номера карты
  if (!rfid.PICC_ReadCardSerial())
    return;

  // Если карта обнаружена и прочитана, формируем UID для вывода
  String uidString = "";
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    // Добавляем ведущий ноль, если байт меньше 0x10
    if (rfid.uid.uidByte[i] < 0x10)
    {
      uidString += "0";
    }
    uidString += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1)
    {
      uidString += ":";
    }
  }
  uidString.toUpperCase(); // Для красоты

  // Вывод UID в последовательный порт
  Serial.print("Card UID: ");
  Serial.println(uidString);

  // Вывод UID на LCD дисплей
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Карта получена!");
  lcd.setCursor(0, 1);
  lcd.print(uidString);

  // Звуковой сигнал
  buzzer.beep(2000, 50, 200, 1); // Частота, длительность, пауза, кол-во раз

  // Завершение работы с меткой
  rfid.PICC_HaltA();

  // Небольшая задержка, чтобы не заспамить дисплей
  delay(2000);

  // Возврат дисплея в режим ожидания
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RFID Читалка");
  lcd.setCursor(0, 1);
  lcd.print("Жду карту...");
}