#include <components/number_input.h>

void NumberInput::clear()
{
    len_ = 0;
    comma_ = false;
    decimals_ = 0;
    kiloPower_ = 0;
}

bool NumberInput::isEmpty() const
{
    return len_ == 0;
}

/*
* Добавляем символ в chars_.
*
* Искоючает возможность:
* - Первого 0;
* - Второй запятой.
*/
bool NumberInput::addChar(char c)
{
    if (len_ >= MAX_LEN) return false;

    if (c == ',') 
    {
        if (comma_) return false;

        if (len_ == 0) {
            chars_[len_++] = '0';
        }

        comma_ = true;
        chars_[len_++] = c;
        return true;
    }

    if (comma_ && (c >= '0' && c <= '9'))
    {
        chars_[len_++] = c;
        decimals_++;

        if (decimals_ > 0 && decimals_ <= 3 && kiloPower_ <= 0) {
            kiloPower_ = 1;
        } else if (decimals_ > 3 && kiloPower_ <= 1) {
            kiloPower_ = 2;
        }

        return true;
    }

    if (!comma_ && (c >= '0' && c <= '9'))
    {
        // Защита от второго нуля в начале ("00")
        if (c == '0' && len_ == 1 && chars_[0] == '0') {
            return false;
        }

        // Замена первого нуля на введенную цифру
        if (c >= '1' && c <= '9' && len_ == 1 && chars_[0] == '0') {
            chars_[0] = c;
            return true;
        }

        char digits[MAX_LEN];
        uint8_t numDigits = 0;
        for (uint8_t i = 0; i < len_; i++) {
            if (chars_[i] >= '0' && chars_[i] <= '9') {
                digits[numDigits++] = chars_[i];
            }
        }

        digits[numDigits++] = c;

        uint8_t requiredSpaces = (numDigits > 0) ? ((numDigits - 1) / 3) : 0;
        if ((numDigits + requiredSpaces) > MAX_LEN) {
            return false;
        }

        len_ = 0;
        for (uint8_t i = 0; i < numDigits; i++) {
            if (i > 0 && (numDigits - i) % 3 == 0) {
                chars_[len_++] = ' ';
            }
            chars_[len_++] = digits[i];
        }

        return true;
    }

    return false;
}

/*
* Меняет текущею степень на следующую по списку:
* 0 -> 1 -> 2 -> 0 -> ...
*
* Данные степени соответствуют умножению на:
* 10^0 -> 10^3 -> 10^6 -> 10^0 -> ...
*/
void NumberInput::nextKiloPower()
{
    kiloPower_++;

    if (kiloPower_ > 2) {
        if (decimals_ > 0 && decimals_ <= 3) {
            kiloPower_ = 1;
        } else if (decimals_ > 3) {
            kiloPower_ = 2;
        } else {
            kiloPower_ = 0;
        }
    }
}

/*
* Удаляем символ из chars_.
*/
void NumberInput::delChar()
{
    if (len_ == 0) return;

    if (comma_) 
    {
        len_--;
        char deleted = chars_[len_];

        if (deleted == ',') {
            comma_ = false;
            decimals_ = 0;
            kiloPower_ = 0;
        } else if (decimals_ > 0) {
            decimals_--;
        }
        return;
    }


    char digits[MAX_LEN];
    uint8_t numDigits = 0;
    for (uint8_t i = 0; i < len_; i++) {
        if (chars_[i] >= '0' && chars_[i] <= '9') {
            digits[numDigits++] = chars_[i];
        }
    }

    if (numDigits > 0) {
        numDigits--;
    }

    len_ = 0;
    for (uint8_t i = 0; i < numDigits; i++) {
        if (i > 0 && (numDigits - i) % 3 == 0) {
            chars_[len_++] = ' ';
        }
        chars_[len_++] = digits[i];
    }

    if (len_ == 0) {
        kiloPower_ = 0;
    }
}

/*
* Исходя из полученной клавиши, решает что делать:
* - при '<' - удалить;
* - при '^' - возвести в степень;
* - в остальных случаях добавить символ.
*/
void NumberInput::enterKey(char key)
{
    if ((key >= '0' && key <= '9') || (key == ',')) addChar(key);
    else if (!isEmpty()) {
        if      (key == '<') delChar();
        else if (key == '^') nextKiloPower();
    }
}

/*
* Переводит строковый input_ в число (uint64).
*/
uint64_t NumberInput::packInto64() const
{
    uint64_t res = 0;

    for (uint8_t i = 0; i < len_; i++) {
        if (chars_[i] == ',' || chars_[i] == ' ') continue;
        res = res * 10 + (chars_[i] - '0');
    }

    int8_t shift = (kiloPower_ * 3) - decimals_;

    while (shift > 0) {
        res *= 10;
        shift--;
    }

    return res;
}