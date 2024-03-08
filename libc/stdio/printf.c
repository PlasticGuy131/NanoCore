#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include "stdio.h"

enum Case
{
    LOWER = 1,
    UPPER = 0
};

static int sputchar(int ic, char* buf, size_t offset)
{
    char c = (char)ic;
    buf[offset] = c;
    return ic;
}

static int print_uint(unsigned i, int (*put)(int), int written)
{
    size_t len = 1;
    unsigned header = 1;
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

static int print_hex(unsigned i, int (*put)(int), int written, enum Case case)
{
    size_t len = 1;
    unsigned header = 1;
    while (i / header >= 16)
    {
        header *= 16;
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
        if (c < 10) { c += '0'; }
        else { c += 'A' + case * 32; }
        if(put(c) == EOF) { return -1; }
        i %= header;
        header /= 16;
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
            int l;
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
                l = print_uint(i, put, written);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'u':
                format++;
                int u = va_arg(arg, int);

                l = print_uint(u, put, written);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'o':
                format++;
                int o = va_arg(arg, int);
                if (o < 0)
                {
                    o = -o;
                    if (written == INT_MAX)
                    {
                        //TODO: Set errno to EOVERFOW.
                        return -1;
                    }
                    if(put((int)'-') == EOF) { return -1; }
                    written++;
                }
                size_t len = 1;
                uint32_t header = 1;
                while (i / header >= 8)
                {
                    header *= 8;
                    len++;
                }

                if (written + len > INT_MAX)
                {
                    //TODO: Set errno to EOVERFOW.
                    return -1;
                }

                for (size_t j = 0; j < len; j++)
                {
                    char ch = (char)(o / header);
                    if(put(ch + '0') == EOF) { return -1; }
                    i %= header;
                    header /= 8;
                }
                written += len;
                break;
            case 'x':
                format++;
                unsigned x = va_arg(arg, unsigned);
                l = print_hex(x, put, written, LOWER);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'X':
                format++;
                unsigned X = va_arg(arg, unsigned);
                l = print_hex(X, put, written, UPPER);
                if (l == -1) { return -1; }
                written += l;
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