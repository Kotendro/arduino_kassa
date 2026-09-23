#include "bank.h"
#include "debug_logger.h"

/*
* Создает аккаунт игрока и возвращает указатель на него.
*/
etl::pair<Account*, const char*> Bank::createNewAcc(const Card& card) {
    if (accounts_.full()) {
        return { nullptr, "ДОСТИГНУТО МАКС.\nКОЛ-ВО ИГРОКОВ" };
    }

    uint8_t id = accounts_.size();
    uint64_t key = card.packInto64();

    // Создаем именно игрока
    auto iter = accounts_.insert({key, Account{AccountType::Player, id, DEFAULT_START_BALANCE}});

    if (!iter.second) {
        return { nullptr, "ОШИБКА СОЗДАНИЯ\nАККАУНТА" };
    }

    return { &(iter.first->second), nullptr };
}

/*
* Выдает указатель на объект счета.
* Если такого счета не существует, пытается создать его.
*/
etl::pair<Account*, const char*> Bank::getOrCreateAcc(const Card& card) {
    uint64_t key = card.packInto64();

    auto iter = accounts_.find(key);
    if (iter != accounts_.end()) {
        return { &(iter->second), nullptr }; 
    }

    return createNewAcc(card);
}

/*
* Выдает указатель на объект счета.
* Если не существует, возвращает nullopt.
*/
etl::pair<Account*, const char*> Bank::getAcc(const Card& card) {
    uint64_t key = card.packInto64();

    auto iter = accounts_.find(key);
    if (iter != accounts_.end()) {
        return { &(iter->second), nullptr }; 
    }   

    return { nullptr, "АККАУНТ\nНЕ СОЗДАН" };
}

/*
* Единая транзакция: Отправитель -> Получатель.
* В зависимости от AccountType, метод сам решает, списывать деньги или нет.
*/
const char* Bank::runTransaction(Account* topAcc, Account* bottomAcc, const NumberInput& input, TransactionDirection direction) {
    // Базовые проверки на null
    if (topAcc == nullptr || bottomAcc == nullptr) {
        return "NULL\n АККАУНТ";
    }
    
    if (topAcc->type == AccountType::None || bottomAcc->type == AccountType::None) {
        return "НЕВЕРНЫЙ ТИП\n АККАУНТА";
    }

    uint64_t amount = input.packInto64();
    if (amount == 0) return nullptr;

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

    return nullptr;
}