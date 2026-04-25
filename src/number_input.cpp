#include <number_input.h>

void NumberInput::clear()
{
    len_ = 0;
    chars_[0] = '\0';
    comma_ = false;
    kiloPower_ = 0;
}

bool NumberInput::isEmpty() const
{
    return len_ == 0;
}

bool NumberInput::addChar(char c)
{
    if (len_ >= MAX_LEN) return false;

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

void NumberInput::nextKiloPower()
{
    kiloPower_++;

    if (kiloPower_ > 2) {
        kiloPower_ = 0;
    }
}

void NumberInput::delChar()
{
    if (len_ == 0) return;

    len_--;

    if (chars_[len_] == ',') {
        comma_ = false;
    }

    chars_[len_] = '\0';
}

void NumberInput::printToSerial() const
{

    if (len_ == 0) {
        Serial.print("None");
    } else {
        Serial.print(chars_);
    }

    if (kiloPower_ == 1) {
        Serial.print('T');
    }
    else if (kiloPower_ == 2) {
        Serial.print('M');
    }

    Serial.println();
}