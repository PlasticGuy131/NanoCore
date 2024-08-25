#include <stddef.h>
#include <string.h>

char* strcpy(char* dest, const char* src)
{
    unsigned i = 0;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = src[i];
    return dest;
}