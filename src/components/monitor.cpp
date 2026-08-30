#include "monitor.h"

Monitor::Monitor(LCD_1602_RUS& lcd) : lcd_(lcd) {}

/* 
* Вспомогательная функция для баланса.
* Она же преобразует баланс в тысячи и милионны при необходимости.
*/
void Monitor::printBalance(const Account* acc) const {
    if (acc->type == AccountType::CentralBank) {
        lcd_.print("БЕЗЛИМИТ ");
        return;
    }

    uint64_t val = acc->balance;
    uint32_t displayVal;
    const char* power = " ";
    
    if (val >= 100000000ULL) {
        if (val >= 100000000000ULL) { 
            displayVal = (uint32_t)(val / 1000000ULL);
            power = "М";
        } else {
            displayVal = (uint32_t)(val / 1000ULL);
            power = "Т";
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
* Отрисовка одной строки экрана ровно на 16 символов.
*/
void Monitor::renderLine(uint8_t row, const Account* acc, bool isReceiver, const NumberInput& input) const {
    lcd_.setCursor(0, row);

    bool showInput = isReceiver && !input.isEmpty();

    // Стрелка (2 символа)
    if (isReceiver && !showInput) lcd_.print("->");
    else lcd_.print("  ");

    // Если счета нет ИЛИ это ЦБ без ввода - прячем остаток строки (14 символов)
    if (acc == nullptr || (acc->type == AccountType::CentralBank && input.isEmpty())) {
        lcd_.print("              "); // Затираем 14 символов
        return;
    }

    // Имя счета (2 символа)
    if (acc->type == AccountType::CentralBank) {
        lcd_.print("ЦБ");
    } else {
        lcd_.print("И");
        lcd_.print(acc->id, 10);
    }

    // Режим ввода или баланс (3 символа)
    if (showInput) lcd_.print("-> ");
    else           lcd_.print(" : ");

    // Ввод или Баланс (9 символов)
    if (showInput) {
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
    if (topAcc != nullptr && topAcc->type == AccountType::CentralBank && bottomAcc == nullptr && input.isEmpty()) {
        renderSystemMessage("ОЖИДАНИЕ\nВВОДА, КАРТЫ");
        return;
    }

    bool topIsReceiver = (dir == TransactionDirection::BottomToTop);
    bool bottomIsReceiver = (dir == TransactionDirection::TopToBottom);

    renderLine(0, topAcc, topIsReceiver, input);
    renderLine(1, bottomAcc, bottomIsReceiver, input);
}

/*
* Вывод любого текста на экран
*/
void Monitor::renderSystemMessage(const char* msg) const
{
    if (msg == nullptr) return;

    char line0[32] = {0};
    char line1[32] = {0};

    const char* newlinePos = strchr(msg, '\n');

    if (newlinePos != nullptr) {
        size_t len0 = newlinePos - msg;
        if (len0 > sizeof(line0) - 1) len0 = sizeof(line0) - 1;
        strncpy(line0, msg, len0);
        line0[len0] = '\0';

        strncpy(line1, newlinePos + 1, sizeof(line1) - 1);
        line1[sizeof(line1) - 1] = '\0';
    } else {
        strncpy(line0, msg, sizeof(line0) - 1);
        line0[sizeof(line0) - 1] = '\0';
    }

    lcd_.setCursor(0, 0);
    lcd_.print("                ");
    lcd_.setCursor(0, 0);
    lcd_.print(line0);

    lcd_.setCursor(0, 1);
    lcd_.print("                ");
    lcd_.setCursor(0, 1);
    lcd_.print(line1);
}