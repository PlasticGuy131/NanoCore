#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include "stdio.h"

static int sputchar(int ic, char* buf, size_t offset)
{
    char c = (char)ic;
    buf[offset] = c;
    return ic;
}

static int print_uint(uint32_t i, int (*put)(int), int written);
{
    size_t len = 1;
    uint32_t header = 1;
    while (i / header >= 10)
    {
        header *= 10;
        len++;
    }

    if (written + len > INT_MAX)
    {
        //TODO: Set errno to EOVERFOW.
        return -1;
    }

    for (size_t j = 0; j < len; j++)
    {
        char c = (char)(i / header);
        if(put(c + '0') == EOF) { return -1; }
        i %= header;
        header /= 10;
    }
    return len;
}

static int aprintf(const char* restrict format, int (*put)(int), va_list arg)
{
    size_t written = 0;

    while (*format != '\0')
    {
        if (written > INT_MAX)
        {
            //TODO: Set errno to EOVERFOW.
            return -1;
        }

        if(*format != '%')
        {
            if(put((int)*format) == EOF) { return -1; }
            written++;
            format++;
            continue;
        }

        format++;
        switch (*format)
        {
            case '%':
                format++;
                if (written == INT_MAX)
                {
                    //TODO: Set errno to EOVERFOW.
                    return -1;
                }
                if(put((int)'%') == EOF) { return -1; }
                written++;
                break;
            case 'c':
                format++;
                if (written == INT_MAX)
                {
                    //TODO: Set errno to EOVERFOW.
                    return -1;
                }
                int c = va_arg(arg, int);
                if(put(c) == EOF) { return -1; }
                written++;
                break;
            case 's':
                format++;
                const char* s = va_arg(arg, const char*);
                size_t len = strlen(s);
                if (written + len > INT_MAX)
                {
                     //TODO: Set errno to EOVERFOW.
                    return -1;
                }
                for (size_t i = 0; i < len; i++)
                {
                    if(put(s[i]) == EOF) { return -1; }
                }
                written += len;
                break;
            case 'd':
            case 'i':
                format++;
                int i = va_arg(arg, int);
                if (i < 0)
                {
                    i = -i;
                    if (written == INT_MAX)
                    {
                        //TODO: Set errno to EOVERFOW.
                        return -1;
                    }
                    if(put((int)'-') == EOF) { return -1; }
                    written++;
                }
                if (print_uint(i) == -1) { return -1; }
                
        }
    }
    return written;
}

int printf(const char* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int written = aprintf(format, &putchar, arg);
    va_end(arg);
    return written;
}