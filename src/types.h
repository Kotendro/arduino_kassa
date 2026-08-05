#pragma once

#include <Arduino.h>
#include "types/event.h"
#include "types/card_uid.h"
#include "types/state.h"

// Может вынесу все остально куда-нибудь
struct Settings {
    uint8_t player_count;
    uint32_t start_balance;
};

struct Player {
    CardUID card;
    uint32_t balance;
};

struct SaveData {
    Settings settings;
    Player players[12];
};
