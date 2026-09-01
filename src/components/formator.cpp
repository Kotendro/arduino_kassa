#include "formator.h"

#include "formator.h"
#include <string.h>

/*
* Собирает итоговый буффер с суфиксов и выравниванием по правой стороне.
*/
static void buildAlignedField(
    char* buffer, size_t bufferSize, const char* text, size_t textLen, const char* suffix
) {
    size_t bufferPos = 0;
    
    size_t padSpaces = (MAX_SYMBOLS > textLen) ? (MAX_SYMBOLS - textLen) : 0;

    while (padSpaces > 0 && bufferPos + 1 < bufferSize) {
        buffer[bufferPos++] = ' ';
        padSpaces--;
    }

    if (text) {
        for (size_t i = 0; i < textLen && bufferPos + 1 < bufferSize; i++) {
            buffer[bufferPos++] = text[i];
        }
    }

    if (suffix) {
        for (size_t i = 0; suffix[i] != '\0' && bufferPos + 1 < bufferSize; i++) {
            buffer[bufferPos++] = suffix[i];
        }
    }

    buffer[bufferPos] = '\0';
}


char* formatBalance(char* buffer, size_t bufferSize, const Account* acc) {
    if (acc->type == AccountType::CentralBank) {
        const char* unlimText = " БЕЗЛИМИТ ";
        
        size_t idx = 0;
        while (unlimText[idx] != '\0' && (idx + 1) < bufferSize) {
            buffer[idx] = unlimText[idx];
            idx++;
        }
        buffer[idx] = '\0';
        return buffer;
    }

    uint64_t raw = acc->balance;
    uint64_t valToFormat = 0;
    const char* suffix = " ";
    char sep3 = ' ';
    char sep6 = ' ';

    /*
    * До 1'000:
    *
    * "      999 "
    */
    if (raw < 1'000ULL) {
        valToFormat = raw;
        suffix = " ";
        sep3 = ' ';
        sep6 = ' ';
    }
    /* 
    * До 1'000'000:
    *
    * "    1,000Т"
    * "  999,000Т"
    * "  999,999Т"
    */
    else if (raw < 1'000'000ULL) {
        valToFormat = raw;
        suffix = "Т";
        sep3 = ',';
        sep6 = ' ';
    }
    /*
    * До 10'000'000'000
    *
    * "    1,000М"
    * "9 999,999М" 
    */
    else if (raw < 10'000'000'000ULL) {
        valToFormat = raw / 1'000ULL;
        suffix = "М";
        sep3 = ',';
        sep6 = ' ';
    }
    /*
    * Все остальное
    *
    * "   10 000М"
    * "9 999 999М"
    */
    else {
        valToFormat = raw / 1'000'000ULL;
        suffix = "М";
        sep3 = ' ';
        sep6 = ' ';
    }


    // Тут мы собираем наше оформатированное число
    // (еще без выравнивания по левой стороне).
    char temp[20];
    uint8_t tempPos = 18;
    temp[19] = '\0';
    uint8_t digitCount = 0;

    if (valToFormat == 0) {
        temp[tempPos] = '0';
        tempPos -= 1;
    } else {
        while (valToFormat > 0) {
            if (digitCount == 3) {
                temp[tempPos] = sep3;
                tempPos -= 1;
            } else if (digitCount == 6) {
                temp[tempPos] = sep6;
                tempPos -= 1;
            }

            uint8_t digit = valToFormat % 10;
            digitCount += 1;
            temp[tempPos] = '0' + digit;
            tempPos -= 1;

            valToFormat /= 10;
        }
    }

    const char* pText = &temp[tempPos + 1];
    size_t textLen = strlen(pText);

    buildAlignedField(buffer, bufferSize, pText, textLen, suffix);
    return buffer;
};

char* formatInput(char* buffer, size_t bufferSize, NumberInputView inputView) {
    const char* suffix = " ";
    if (inputView.kiloPower_ == 1)      suffix = "Т";
    else if (inputView.kiloPower_ == 2) suffix = "М";

    buildAlignedField(buffer, bufferSize, inputView.chars_, inputView.len_, suffix);
    return buffer;
};