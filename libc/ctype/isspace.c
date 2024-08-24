#include <ctype.h>

int isspace(int ch)
{
    return ch == ' ' || ch == '\f' || ch == '\n' || ch == '\r' || ch == '\r' || ch == '\t' || ch == '\v';
}