#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <OnewireKeypad.h>

// LiquidCrystal_I2C lcd(0x27, 16, 2);

char KEYS[] = {
  '/','*','-','+',
  '9','6','3','=',
  '8','5','2','.',
  '7','4','1','0'
};

uint16_t ADC_THRESHOLDS[] = {
    767, 426, 299, 217,
    165, 141, 124, 106,
    92, 85, 78, 71,
    64, 60, 57, 53
};

OnewireKeypad <Print, 16> KP(Serial, KEYS, ADC_THRESHOLDS, 4, 4, A1);

void setup() {
    Serial.begin(9600);
}

void loop() {
    switch (KP.keyState()) {
        case PRESSED: {
            char key = KP.getkey();
            if (key != NO_KEY) {
                Serial << "Pressed: " << key << "\n";
            }
            break;
        }

        case HELD: {
            char key = KP.getkey();
            if (key != NO_KEY) {
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
}