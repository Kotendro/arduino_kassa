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
    static constexpr uint16_t DEFAULT_START_BALANCE = 0;

    etl::map<uint64_t, Player, MAX_ACCOUNTS> accounts_;
public:
    etl::optional<Player&> createNewAcc(const Card& card);
    etl::optional<Player&> getOrCreateAcc(const Card& card);

    void runCashierTransaction(const Card& card, const NumberInput& input);
    void runFPS();
};

