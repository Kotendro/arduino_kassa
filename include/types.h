#include <stdint.h>
#include <Arduino.h>
#include <card_uid.h>

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
