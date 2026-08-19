#include "bank.h"
    
/*
* Создает игрока и возвращает указатель на него.
*/
etl::optional<Player*> Bank::createNewAcc(const Card& card) {
    if (accounts_.full()) {
        return etl::nullopt;
    }

    uint8_t id = accounts_.size();
    uint64_t key = card.packInto64();

    auto iter = accounts_.insert({key, Player{id, DEFAULT_START_BALANCE}});

    if (iter.second) {
        return &(iter.first->second);
    }

    return etl::nullopt;
}

/*
* Выдает указатель на объект игрока.
* Если такого игрока не существует, пытается создать его.
*/
etl::optional<Player*> Bank::getOrCreateAcc(const Card& card) {
    uint64_t key = card.packInto64();

    auto iter = accounts_.find(key);
    if (iter != accounts_.end()) {
        return &(iter->second); 
    }

    return createNewAcc(card);
}

/*
* Перевод денежных средств между ЦБ и человеком.
* Защита от переполнения баланса и от ухода в минус.
*
* [1-2 сценарий]
*/
void Bank::runOneSideTransaction(const Card& card, const NumberInput& input) {
    etl::optional<Player*> player = getOrCreateAcc(card);
    if (!player.has_value()) {
        Serial.println(F("ERROR: player doesn't exist"));
        return;
    };

    uint64_t amount = input.packInto64();
    Player* p = player.value();

    if (!input.reverseDirection) {
        /*
        * True -> У игрока еще есть место до максимального баланса,
        * поэтому деньги можно просто добавить.
        * 
        * Fasle-> У игрока баланс после пополнение выйдет за максимум,
        * поэтому просто приравниваем баланс к максимально возможному балансу. 
        */
        if (MAX_BALANCE - p->balance > amount) {
            p->balance += amount;
        } else {
            p->balance = MAX_BALANCE;
        }
    } else {
        /*
        * True -> У игрока после списания баланс выйдет в минус,
        * поэтому просто приравниваем баланс к 0.
        * 
        * False-> У игрока баланс не выйдет в минус попсле списания,
        * поэтому просто минусуем необходимую сумму
        */
        if (p->balance < amount) {
            p->balance = 0;
        } else {
            p->balance -= amount;
        }
    }
}


/*
* Перевод денежных средств между 2 людьми.
* Защита от переполнения баланса и от ухода в минус.
*
* [3 сценарий]
*/
void Bank::runTwoSideTransaction(const Card& firstCard, const Card& secondCard, const NumberInput& input) {
    etl::optional<Player*> player1 = getOrCreateAcc(firstCard);
    etl::optional<Player*> player2 = getOrCreateAcc(secondCard);
    if (!player1.has_value() || !player2.has_value()) {
        Serial.println(F("ERROR: player doesn't exist"));
        return;
    };

    uint64_t amount = input.packInto64();

    Player* pFrom = !input.reverseDirection ? player1.value() : player2.value();
    Player* pTo   = !input.reverseDirection ? player2.value() : player1.value();

    uint64_t actualAmount = 0; // Сумма, которую получает pTo.

    /*
    * True -> Баланс отправителя меньше заданного перевода,
    * поэтому отправляем что можем (actualAmount) и
    * обнуляем его баланс.
    * 
    * False-> Баланс отправителя больше заданного перевода.
    */
    if (pFrom->balance < amount) {
        actualAmount = pFrom->balance;
        pFrom->balance = 0;
    } else {
        actualAmount = amount;
        pFrom->balance -= amount;
    }

    /*
    * True -> У игрока еще есть место до максимального баланса,
    * поэтому деньги можно просто добавить.
    * 
    * Fasle-> У игрока баланс после пополнение выйдет за максимум,
    * поэтому просто приравниваем баланс к максимально возможному балансу. 
    */
    if (MAX_BALANCE - pTo->balance > actualAmount) {
        pTo->balance += actualAmount;
    } else {
        pTo->balance = MAX_BALANCE;
    }
}