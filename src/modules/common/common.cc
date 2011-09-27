#include <cmath>

#include "common.h"

BOREALIS_NAMESPACE_BEGIN

string get_formatted_number(double number, int width, int precision)
{
    char _buffer[40];
    string s;
    s =  s+ "%" + width + "." + precision + "f" + '\0';
    sprintf(_buffer, s.c_str(), number);
    return string(_buffer);
}

string get_formatted_number(double number, int precision)
{
    int digits = (int)(log10(number));
    if (digits < 0)
        digits = 0;
    digits += (1+precision);
    return get_formatted_number(number, digits, precision);
}

string get_formatted_number(int number, int width)
{
    char _buffer[40];
    string s;
    s = s + "%" + width + "d" + '\0';
    sprintf(_buffer, s.c_str(), number);
    return string(_buffer);
}

BOREALIS_NAMESPACE_END
