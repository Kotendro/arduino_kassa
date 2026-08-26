#include "monitor.h"

Monitor::Monitor(LCD_1602_RUS& lcd) : lcd_(lcd) {}

/* 
* Вспомогательная функция для баланса.
* Она же преобразует баланс в тысячи и милионны при необходимости.
*/
void Monitor::printBalance(const Account* acc) const {
    if (acc->type == AccountType::CentralBank) {
        lcd_.print("Безлимит ");
        return;
    }

    uint64_t val = acc->balance;
    uint32_t displayVal;
    const char* power = " ";
    
    if (val >= 100000000ULL) {
        if (val >= 100000000000ULL) { 
            displayVal = (uint32_t)(val / 1000000ULL);
            power = "м";
        } else {
            displayVal = (uint32_t)(val / 1000ULL);
            power = "т";
        }
    } else {
        displayVal = (uint32_t)val;
    }

    char buf[16];
    sprintf(buf, "%8lu", displayVal); 
    lcd_.print(buf);
    lcd_.print(power);
}

/*
* Отрисовка одной строки экрана ровно на 16 символов .
*/
void Monitor::renderLine(uint8_t row, const Account* acc, bool isReceiver, const NumberInput& input) const {
    lcd_.setCursor(0, row);
    
    if (acc == nullptr) {
        lcd_.print("                "); // Затираем строку пробелами
        return;
    }

    // Стрелка (2 символа)
    if (isReceiver) lcd_.print("->");
    else lcd_.print("  ");
    
    // Имя счета (5 символов)
    if (acc->type == AccountType::CentralBank) {
        lcd_.print("ЦБ : ");
    } else {
        lcd_.print("И");
        lcd_.print(acc->id, 10); 
        lcd_.print(" : ");
    }

    // Ввод или Баланс (9 символов)
    if (isReceiver && !input.isEmpty()) {
        char inBuf[12]; 
        input.getRightAlignedStr(inBuf);
        lcd_.print(inBuf);
    } else {
        printBalance(acc);
    }
}

/*
* Отрисовка всего экрана.
*/
void Monitor::renderTransactionScreen(const Account* topAcc, const Account* bottomAcc, const NumberInput& input, TransactionDirection dir) const {
    bool topIsReceiver = (dir == TransactionDirection::BottomToTop);
    bool bottomIsReceiver = (dir == TransactionDirection::TopToBottom);

    renderLine(0, topAcc, topIsReceiver, input);
    renderLine(1, bottomAcc, bottomIsReceiver, input);
}