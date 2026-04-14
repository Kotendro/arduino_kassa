#include <stdint.h>

struct Settings {
    uint8_t player_count;
    uint32_t start_balance;
};

struct Player {
    // метка карты
    uint32_t balance;
};

struct SaveData {
    Settings settings;
    // Здесь максимальное количество игроков
    Player players[12];
};
