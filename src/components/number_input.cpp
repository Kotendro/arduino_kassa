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

    if (c == '0')
    {
        if (len_ == 1 && chars_[0] == '0')
            return false;

        chars_[len_++] = c;

        if (comma_) 
        {
            decimals_++;
            if (decimals_ > 0 && decimals_ <= 3 && kiloPower_ <= 0) {
                kiloPower_ = 1;
            } else if (decimals_ > 3 && kiloPower_ <= 1) {
                kiloPower_ = 2;
            }
        }

        return true;
    }

    if (c >= '1' && c <= '9') 
    {
        if (len_ == 1 && chars_[0] == '0')
        {
            chars_[0] = c;
            return true;
        }

        if (comma_) 
        {
            decimals_++;
            if (decimals_ > 0 && decimals_ <= 3 && kiloPower_ <= 0) {
                kiloPower_ = 1;
            } else if (decimals_ > 3 && kiloPower_ <= 1) {
                kiloPower_ = 2;
            }
        }

        chars_[len_++] = c;
        return true;
    }

    if (c == ',') 
    {
        if (comma_) 
            return false;

        if (len_ == 0)
            chars_[len_++] = '0';

        comma_ = true;
        chars_[len_++] = c;
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

    len_--;

    if (chars_[len_] == ',') {
        comma_ = false;
    } else if (comma_) {
        decimals_--;
    }
}

/*
* Записывает отформатированный ввод (8 цифр + степень) в переданный буфер
*/
void NumberInput::getRightAlignedStr(char* buffer) const
{
    uint8_t index = 0;
    
    for (uint8_t i = 0; i < 8 - len_; i++) buffer[index++] = ' ';
    for (uint8_t i = 0; i < len_; i++) buffer[index++] = chars_[i];
    
    buffer[index] = '\0';
    
    if (kiloPower_ == 1) strcat(buffer, "т");
    else if (kiloPower_ == 2) strcat(buffer, "м");
    else strcat(buffer, " ");
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
    else if (key == '<') delChar();
    else if (key == '^') nextKiloPower();
}

/*
* Переводит строковый input_ в число (uint64).
*/
uint64_t NumberInput::packInto64() const
{
    uint64_t res = 0;

    for (uint8_t i = 0; i < len_; i++) {
        if (chars_[i] == ',') continue;
        res = res * 10 + (chars_[i] - '0');
    }

    int8_t shift = (kiloPower_ * 3) - decimals_;

    while (shift > 0) {
        res *= 10;
        shift--;
    }

    return res;
}