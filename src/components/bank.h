#pragma once

#include <etl/map.h>
#include <etl/utility.h>
#include "types/transactionDirection.h"
#include "types/card.h"
#include "types/account.h"
#include "components/number_input.h"

class Bank
{
private:
    static constexpr uint8_t MAX_ACCOUNTS = 6;
    static constexpr uint64_t DEFAULT_START_BALANCE = 1000;
    static constexpr uint64_t MAX_BALANCE = 99999999000000;

    etl::map<uint64_t, Account, MAX_ACCOUNTS> accounts_;
public:
    etl::pair<Account*, const char*> createNewAcc(const Card& card);
    etl::pair<Account*, const char*> getOrCreateAcc(const Card& card);
    etl::pair<Account*, const char*> getAcc(const Card& card);

    const char* runTransaction(Account* topAcc, Account* bottomAcc, const NumberInput& input, TransactionDirection direction);
};