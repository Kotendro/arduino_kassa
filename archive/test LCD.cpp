#include <Arduino.h>
// Подключаем библиотеку для работы с LCD дисплеем по шине I2C
#include <Wire.h>
// Тип подключения дисплея: 1 - по шине I2C, 2 - десятиконтактное. Обязательно указывать ДО подключения библиотеки
// Если этого не сделать, при компиляции возникнет ошибка: "LCD type connect has not been declared"
#define _LCD_TYPE 1
#include <LCD_1602_RUS_ALL.h>

#define LCD_ADDR 0x27

LCD_1602_RUS lcd(LCD_ADDR, 16, 2);

void setup()
{
    Serial.begin(9600);

    // Инициализация дисплея
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("RFID Читалка");
    lcd.setCursor(0, 1);
    lcd.print("Жду карту...");
}

void loop()
{
}