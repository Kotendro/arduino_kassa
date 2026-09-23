#pragma once

#include <Arduino.h>
#include "components/number_input.h"
#include "components/bank.h"

// Максимум символов без учета суффикса (" ", "Т", "М")
static constexpr int MAX_SYMBOLS = 9;

char* formatBalance(char* buffer, size_t bufferSize, const Account* acc);
char* formatInput(char* buffer, size_t bufferSize, NumberInputView inputView);