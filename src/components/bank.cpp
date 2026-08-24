#include "bank.h"
#include "debug_logger.h"

/*
* Создает аккаунт игрока и возвращает указатель на него.
*/
etl::optional<Account*> Bank::createNewAcc(const Card& card) {
    if (accounts_.full()) {
        return etl::nullopt;
    }

    uint8_t id = accounts_.size();
    uint64_t key = card.packInto64();

    // Создаем именно игрока
    auto iter = accounts_.insert({key, Account{AccountType::Player, id, DEFAULT_START_BALANCE}});

    if (iter.second) {
        return &(iter.first->second);
    }

    return etl::nullopt;
}

/*
* Выдает указатель на объект счета.
* Если такого счета не существует, пытается создать его.
*/
etl::optional<Account*> Bank::getOrCreateAcc(const Card& card) {
    uint64_t key = card.packInto64();

    auto iter = accounts_.find(key);
    if (iter != accounts_.end()) {
        return &(iter->second); 
    }

    return createNewAcc(card);
}

/*
* Выдает указатель на объект счета.
* Если не существует, возвращает nullopt.
*/
etl::optional<Account*> Bank::getAcc(const Card& card) {
    uint64_t key = card.packInto64();

    auto iter = accounts_.find(key);
    if (iter != accounts_.end()) {
        return &(iter->second); 
    }   

    return etl::nullopt;
}

/*
* Единая транзакция: Отправитель -> Получатель.
* В зависимости от AccountType, метод сам решает, списывать деньги или нет.
*/
void Bank::runTransaction(Account* topAcc, Account* bottomAcc, const NumberInput& input, TransactionDirection direction) {
    // Базовые проверки на null
    if (topAcc == nullptr || bottomAcc == nullptr) {
        DEBUG_ERROR(F("Account pointer is null"));
        return;
    }
    
    if (topAcc->type == AccountType::None || bottomAcc->type == AccountType::None) {
        DEBUG_ERROR(F("Invalid account type"));
        return;
    }

    uint64_t amount = input.packInto64();
    if (amount == 0) return;

    Account* from = (direction == TransactionDirection::TopToBottom) ? topAcc : bottomAcc;
    Account* to   = (direction == TransactionDirection::TopToBottom) ? bottomAcc : topAcc;

    uint64_t actualAmount = amount; 

    if (from->type == AccountType::Player) {
        if (from->balance < amount) {
            actualAmount = from->balance;
            from->balance = 0;
        } else {
            from->balance -= amount;
        }
    }

    if (to->type == AccountType::Player) {
        if (MAX_BALANCE - to->balance > actualAmount) {
            to->balance += actualAmount;
        } else {
            to->balance = MAX_BALANCE;
        }
    }
}