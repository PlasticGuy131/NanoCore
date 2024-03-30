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

static int get_exp(double* d)
{
    int exp = 0;
    while (*(d) >= 10.0)
    {
        *(d) /= 10.0;
        exp++;
    }
    while (*(d) < 1.0)
    {
        *(d) *= 10.0;
        exp--;
    }
    return exp;
}

static int sputchar(int ic)
{
    char c = (char)ic;
    buf[offset] = c;
    offset++;
    return ic;
}

static size_t print_uint(unsigned i, int (*put)(int), size_t written, unsigned max)
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

static size_t print_hex(unsigned i, int (*put)(int), size_t written, unsigned max, enum Case hcase)
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

static size_t print_float(double f, int (*put)(int), size_t written, unsigned max, unsigned dp)
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

    int l = print_uint(i, put, written, max);
    if (l == -1) { return -1; }
    written += l;

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

static size_t print_exp(double f, int (*put)(int), size_t written, unsigned max, unsigned dp, enum Case ecase)
{
    int exp = get_exp(&f);

    int l = print_float(f, put, written, max, dp);
    if (l == -1) { return -1; }
    written += l;

    if (written + 4 > max)
    {
        //TODO: Set errno to EOVERFOW.
        return -1;
    }
    if(put((ecase == UPPER) ? 'E' : 'e') == EOF) { return -1; }
    if(put((exp < 0) ? '-' : '+') == EOF) { return -1; }
    if (exp < 0) { exp *= -1; }
    if (exp < 10) { if(put('0') == EOF) { return -1; } }
    l = print_uint((unsigned) exp, put, written, max);
    if (l == -1) { return -1; }
    written += 4;
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
                l = print_hex(x, put, written, max, LOWER);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'X':
                format++;
                unsigned X = va_arg(arg, unsigned);
                l = print_hex(X, put, written, max, UPPER);
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
                l = print_hex(p, put, written, max, UPPER);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'f':
            case 'F':
                format++;
                double f = va_arg(arg, double);

                l = print_float(f, put, written, max, 6);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'e':
                format++;
                double e = va_arg(arg, double);

                l = print_exp(e, put, written, max, 6, LOWER);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'E':
                format++;
                double E = va_arg(arg, double);

                l = print_exp(E, put, written, max, 6, UPPER);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'h':
                format++;
                double h = va_arg(arg, double);

                double test_h = h;
                int exp_h = get_exp(&test_h);
                if (exp_h < -4 || exp_h >= 6)
                {
                    l = print_exp(h, put, written, max, 6, LOWER);                    
                }
                else
                {
                    l = print_float(h, put, written, max, 6);
                }
                written += l;
                break;
            case 'H':
                format++;
                double H = va_arg(arg, double);

                double test_H = H;
                int exp_H = get_exp(&test_H);
                if (exp_H < -4 || exp_H >= 6)
                {
                    l = print_exp(H, put, written, max, 6, UPPER);                    
                }
                else
                {
                    l = print_float(H, put, written, max, 6);
                }
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
