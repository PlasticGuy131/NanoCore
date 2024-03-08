#include <limits.h>
#include <stdarg.h>
#include <string.h>

#include "stdio.h"

static int sputchar(int ic, char* buf, size_t offset)
{
    char c = (char)ic;
    buff[offset] = c;
    return ic;
}

static char* aprintf(const char* restrict format, int (*put)(int), va_list arg)
{
    size_t written = 0;

    while (*format != '/0')
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
                int c = (int)'%';
                if(put(c) == EOF) { return -1; }
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
                    if(put(c) == EOF) { return -1; }
                }
                written += len;
                break;
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