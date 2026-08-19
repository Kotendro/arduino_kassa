#pragma once

#include <etl/map.h>
#include <etl/optional.h>
#include "types/card.h"
#include "types/player.h"
#include "components/number_input.h"

class Bank
{
private:
    static constexpr uint8_t MAX_ACCOUNTS = 6;
    static constexpr uint64_t DEFAULT_START_BALANCE = 0;
    static constexpr uint64_t MAX_BALANCE = 99999999000000;

    etl::map<uint64_t, Player, MAX_ACCOUNTS> accounts_;
public:
    etl::optional<Player*> createNewAcc(const Card& card);
    etl::optional<Player*> getOrCreateAcc(const Card& card);

    void runOneSideTransaction(const Card& card, const NumberInput& input);
    void runTwoSideTransaction(const Card& firstCard, const Card& secondCard, const NumberInput& input);
};

