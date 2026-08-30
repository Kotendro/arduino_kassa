#pragma once

#include "config.h"

#include <LCD_1602_RUS_ALL.h>
#include "types/account.h"
#include "types/transactionDirection.h"
#include "components/number_input.h"


class Monitor
{
private:
    LCD_1602_RUS& lcd_;

    void printBalance(const Account* acc) const;
    void renderLine(uint8_t row, const Account* acc, bool isReceiver, const NumberInput& input) const;

public:
    Monitor(LCD_1602_RUS& lcd);

    void renderTransactionScreen(const Account* topAcc, const Account* bottomAcc, const NumberInput& input, TransactionDirection dir) const;
    void renderSystemMessage(const char* text) const;

};