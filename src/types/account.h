#pragma once

#include <Arduino.h>

enum class AccountType { 
    None, 
    Player, 
    CentralBank 
};

struct Account
{
    AccountType type = AccountType::None;
    uint8_t id = 0;
    uint64_t balance = 0;

    Account(AccountType t = AccountType::None, uint8_t i = 0, uint64_t b = 0)
        : type(t), id(i), balance(b) {}
};