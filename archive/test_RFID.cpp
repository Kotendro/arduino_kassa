#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

void setup()
{
  Serial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();

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

  // Завершение работы с меткой
  rfid.PICC_HaltA();

  // Небольшая задержка, чтобы не заспамить дисплей
  delay(2000);
}