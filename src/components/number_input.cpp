#include <components/number_input.h>

void NumberInput::clear()
{
    len_ = 0;
    chars_[0] = '\0';
    comma_ = false;
    kiloPower_ = 0;
    reverseDirection = false;
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
    if (len_ >= MAX_LEN-1) return false; // учитываем элемент '\0'

    if (c == '0')
    {
        if (len_ == 1 && chars_[0] == '0')
            return false;

        chars_[len_++] = c;
        chars_[len_] = '\0';
        return true;
    }

    if (c >= '1' && c <= '9') 
    {
        if (len_ == 1 && chars_[0] == '0')
        {
            chars_[0] = c;
            return true;
        }

        chars_[len_++] = c;
        chars_[len_] = '\0';
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
        chars_[len_] = '\0';
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
        kiloPower_ = 0;
    }
}

void NumberInput::switchDirection()
{
    reverseDirection = !reverseDirection;
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
    }

    chars_[len_] = '\0';
}

/*
* DEBUG
*
* Вывод текущего резултата в консоль.
* Учитывает степень
*/
void NumberInput::printToSerial() const
{
    if (len_ == 0) {
        Serial.println("None");
        return;
    }

    if (!reverseDirection) Serial.print("->");
    else Serial.print("<-");

    Serial.print(chars_);

    if (kiloPower_ == 1) Serial.print('T');
    else if (kiloPower_ == 2) Serial.print('M');

    Serial.println();
}

/*
* Исходя из полученной клавиши, решает что делать:
* - при '<' - удалить;
* - при '^' - возвести в степень;
* - при '+' - поменять направление;
* - в остальных случаях добавить символ.
*/
void NumberInput::enterKey(char key)
{
    if ((key >= '0' && key <= '9') || (key == ',')) addChar(key);
    else if (key == '<') delChar();
    else if (key == '^') nextKiloPower();
    else if (key == '+') switchDirection();
}