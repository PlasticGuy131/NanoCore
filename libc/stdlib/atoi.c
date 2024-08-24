#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

int atoi(const char* str)
{
    while (isspace(*str)) { str++; }

    bool positive = true;
    if (*str == '-')
    {
        positive = false;
        str++;
    }
    else if (*str == '+')
    {
        str++;
    }

    int value = 0;
    while (isdigit(*str))
    {
        value *= 10;
        value -= *str - '0';
    }

    return positive ? -value : value;
}