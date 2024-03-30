#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

enum Case
{
    LOWER = 1,
    UPPER = 0
};

char* buf;
int offset;

static int sputchar(int ic)
{
    char c = (char)ic;
    buf[offset] = c;
    offset++;
    return ic;
}

static int print_uint(unsigned i, int (*put)(int), int written, unsigned max)
{
    size_t len = 1;
    unsigned header = 1;
    while (i / header >= 10)
    {
        header *= 10;
        len++;
    }

    if (written + len > max)
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

static int print_hex(unsigned i, int (*put)(int), int written, enum Case hcase, unsigned max)
{
    size_t len = 1;
    unsigned header = 1;
    while (i / header >= 16)
    {
        header *= 16;
        len++;
    }

    if (written + len > max)
    {
        //TODO: Set errno to EOVERFOW.
        return -1;
    }

    for (size_t j = 0; j < len; j++)
    {
        char c = (char)(i / header);
        if (c < 10) { c += '0'; }
        else { c += 'A' - 10 + hcase * 32; }
        if(put(c) == EOF) { return -1; }
        i %= header;
        header /= 16;
    }
    return len;
}

static int print_float(double f, int (*put)(int), int written, unsigned max, unsigned dp)
{
    if (f < 0)
    {
        if (written == max)
        {
            // TODO: Set errno to EOVERFOW.
            return -1;
        }
        if (put((int)'-') == EOF) { return -1; }
        written++;
        f *= -1;
    }
    unsigned i = (unsigned) f;
    f -= i;

    bool rounded = false;
    bool round = true;
    double test_f = f;
    for (size_t j = 0; j < dp+1; j++)
    {
        test_f *= 10.0;
        unsigned test_c = test_f;
        test_f -= test_c;
        unsigned limit = (j == dp) ? 5 : 9;
        if (test_c < limit)
        {
            round = false;
            break;
        }
    }

    if (round)
    {
        i++;
        rounded = true;
    }

    written += print_uint(i, put, written, max);

    if (written == max)
    {
        //TODO: Set errno to EOVERFOW.
        return -1;
    }
    if(put((int)'.') == EOF) { return -1; }
    written++;

    while (dp > 0)
    {
        dp--;
        if (written == max)
        {
            //TODO: Set errno to EOVERFOW.
            return -1;
        }

        if (rounded)
        {
            if(put('0') == EOF) { return -1; }
            written++;
            continue;
        }

        f *= 10;
        unsigned c = f;
        f -= c;

        bool round = true;
        double test_f = f;
        for (size_t i = 0; i < dp+1; i++)
        {
            test_f *= 10.0;
            unsigned test_c = test_f;
            test_f -= test_c;
            unsigned limit = (i == dp) ? 5 : 9;
            if (test_c < limit)
            {
                round = false;
                break;
            }
        }

        if (round)
        {
            c++;
            rounded = true;
        }

        if(put(c + '0') == EOF) { return -1; }
        written++;
    }
    return written;
}

static int vaprintf(const char* restrict format, int (*put)(int), unsigned max, va_list arg)
{
    //TODO: Flags and such
    size_t written = 0;

    while (*format != '\0')
    {
        if (written > max)
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
            size_t len;
            case '%':
                format++;
                if (written == max)
                {
                    //TODO: Set errno to EOVERFOW.
                    return -1;
                }
                if(put((int)'%') == EOF) { return -1; }
                written++;
                break;
            case 'c':
                format++;
                if (written == max)
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
                len = strlen(s);
                if (written + len > max)
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
                    if (written == max)
                    {
                        //TODO: Set errno to EOVERFOW.
                        return -1;
                    }
                    if(put((int)'-') == EOF) { return -1; }
                    written++;
                }
                l = print_uint(i, put, written, max);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'u':
                format++;
                int u = va_arg(arg, int);

                l = print_uint(u, put, written, max);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'o':
                format++;
                int o = va_arg(arg, int);
                if (o < 0)
                {
                    o = -o;
                    if (written == max)
                    {
                        //TODO: Set errno to EOVERFOW.
                        return -1;
                    }
                    if(put((int)'-') == EOF) { return -1; }
                    written++;
                }
                len = 1;
                unsigned header = 1;
                while (o / header >= 8)
                {
                    header *= 8;
                    len++;
                }

                if (written + len > max)
                {
                    //TODO: Set errno to EOVERFOW.
                    return -1;
                }

                for (size_t j = 0; j < len; j++)
                {
                    char ch = (char)(o / header);
                    if(put(ch + '0') == EOF) { return -1; }
                    o %= header;
                    header /= 8;
                }
                written += len;
                break;
            case 'x':
                format++;
                unsigned x = va_arg(arg, unsigned);
                l = print_hex(x, put, written, LOWER, max);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'X':
                format++;
                unsigned X = va_arg(arg, unsigned);
                l = print_hex(X, put, written, UPPER, max);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'p':
                format++;
                unsigned p = va_arg(arg, unsigned);
                if (written + 2 > max)
                {
                    //TODO: Set errno to EOVERFOW.
                    return -1;
                }

                if(put((int)'0') == EOF) { return -1; }
                if(put((int)'x') == EOF) { return -1; }
                l = print_hex(p, put, written, UPPER, max);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'f':
                format++;
                double f = va_arg(arg, double);

                l = print_float(f, put, written, max, 6);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'n':
                format++;
                int* n = va_arg(arg, int*);
                *n = written;
        }
    }
    return written;
}

int vprintf(const char* restrict format, va_list arg)
{
    return vaprintf(format, &putchar, INT_MAX, arg);
}

int vsprintf(char* s, const char* restrict format, va_list arg)
{
    buf = s;
    offset = 0;
    return vaprintf(format, &sputchar, INT_MAX, arg);
}

int vsnprintf(char* s, const char* restrict format, size_t n, va_list arg)
{
    buf = s;
    offset = 0;
    return vaprintf(format, &sputchar, n, arg);
}

int printf(const char* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int written = vprintf(format, arg);
    va_end(arg);
    return written;
}

int sprintf(char* s, const char* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int written = vsprintf(s, format, arg);
    va_end(arg);
    return written;
}

int snprintf(char* s, const char* restrict format, size_t n, ...)
{
    va_list arg;
    va_start(arg, n);
    int written = vsnprintf(s, format, n, arg);
    va_end(arg);
    return written;
}
