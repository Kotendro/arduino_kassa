#include "monitor.h"

Monitor::Monitor(LCD_1602_RUS& lcd) : lcd_(lcd) {}

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
    if (showInput) lcd_.print("->");
    else           lcd_.print(" :");

    // Ввод или Баланс (10 символов)
    uint8_t bufferSize = 24;
    char buffer[bufferSize];
    
    if (showInput)
        formatInput(buffer, bufferSize, input.getNumberInputView());
    else
        formatBalance(buffer, bufferSize, acc);

    lcd_.print(buffer);
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