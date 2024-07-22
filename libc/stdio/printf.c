#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static const unsigned char PRINTF_FLAG_ALT = 1;
static const unsigned char PRINTF_FLAG_ZERO = 2;
static const unsigned char PRINTF_FLAG_LEFT = 4;
static const unsigned char PRINTF_FLAG_SPACE = 8;
static const unsigned char PRINTF_FLAG_SIGN = 16;

enum Case
{
    LOWER = 1,
    UPPER = 0
};

char* buf;
int offset;

static int get_exp(double* d, unsigned base)
{
    int exp = 0;
    while (*(d) >= base)
    {
        *(d) /= base;
        exp++;
    }
    while (*(d) < 1.0)
    {
        *(d) *= base;
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

static int print_uint(unsigned i, int (*put)(int), size_t written, unsigned max)
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
        errno = EOVERFLOW;
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

static int print_hex(unsigned i, int (*put)(int), size_t written, unsigned max, enum Case hcase)
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
        errno = EOVERFLOW;
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

static int print_float(double f, int (*put)(int), size_t written, unsigned max, unsigned dp, bool truncate)
{
    if (f < 0)
    {
        if (written == max)
        {
            errno = EOVERFLOW;
            return -1;
        }
        if (put('-') == EOF) { return -1; }
        written++;
        f *= -1;
    }
    unsigned integer = f;
    f -= integer;

    char* str = (char*)malloc((dp + 1) * sizeof(char));
    
    for (size_t i = 0; i < dp+1; i++)
    {
        f *= 10;
        str[i] = (char)(int)f;
        f -= (int)f;
    }

    size_t offset = dp;
    bool round = str[offset] >= 5;
    while (round)
    {
        offset--;
        if (str[offset] < 9)
        {
            str[offset]++;
            round = false;
        }
        else
        {
            str[offset] = 0;
        }
    }

    if(round) { integer++; }

    int l = print_uint(integer, put, written, max);
    if (l == -1)
    {
        free(str);
        return -1;
    }
    written += l;

    if (false)
    {
        offset = dp;
        while (offset > 0)
        {
            offset--;
            if(str[offset] == 0) { str[offset] = 10; }
            else { break; }
        }

        if (offset == 0)
        {
            free(str);
            return written;
        }
    }

    if (dp == 0)
    {
        free(str);
        return written;
    }
    if (written == max)
    {
        errno = EOVERFLOW;
        return -1;
    }
    if (put('.') == EOF)
    {
        free(str);
        return -1;
    }
    written++;

    for (size_t i = 0; i < dp; i++)
    {
        if(str[i] == 10) { break; }
        if (written == max)
        {
            errno = EOVERFLOW;
            free(str);
            return -1;
        }
        if (put(str[i] + '0') == EOF)
        {
            free(str);
            return -1;
        }
        written++;
    }

    free(str);
    return written;

/*
    bool rounded = false;
    bool round = true;
    bool stop = true;
    double test_f = f;
    for (size_t j = 0; j < dp+1; j++)
    {
        test_f *= 10.0;
        unsigned test_c = test_f;
        test_f -= test_c;

        if ((test_c != 0 && j < dp) || (test_c >= 5 && j == dp))
        {
            stop = false;
        }
        unsigned limit = (j == dp) ? 5 : 9;
        if (test_c < limit)
        {
            round = false;
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

    if (truncate && stop) { return written; }

    if (written == max)
    {
        errno = EOVERFLOW;
        return -1;
    }
    if(put((int)'.') == EOF) { return -1; }
    written++;
    
    bool cont = true;
    while (dp > 0 && !(truncate && !cont))
    {
        dp--;
        if (written == max)
        {
            errno = EOVERFLOW;
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
            cont = cont && (test_c == 0);
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
*/
}

static int print_exp(double f, int (*put)(int), size_t written, unsigned max, unsigned dp, bool truncate, enum Case ecase)
{
    int exp = get_exp(&f, 10);

    int l = print_float(f, put, written, max, dp, truncate);
    if (l == -1) { return -1; }
    written += l;

    if (written + 4 > max)
    {
        errno = EOVERFLOW;
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

static int print_float_hex_exp(int exp, int (*put)(int), size_t written, unsigned max, enum Case acase)
{
    if (put((acase == UPPER) ? 'P' : 'p') == EOF) { return -1; }
    if (put((exp >= 0) ? '+' : '-') == EOF) { return -1; }
    if (exp < 0) { exp *= -1; }
    if (exp < 10) { if (put('0') == EOF) { return -1; } }
    int l = print_uint(exp, put, written, max);
    if (l == -1) { return -1; }
    written += l;
    return written;
}

static int print_float_hex(double f, int (*put)(int), size_t written, unsigned max, bool trunc, unsigned dp, bool point, enum Case acase)
{
    if (f < 0)
    {
        if (written == max)
        {
            errno = EOVERFLOW;
            return -1;
        }
        if (put('-') == EOF) { return -1; }
        written++;
        f *= -1;
    }

    int exp = get_exp(&f, 2);

    if (written + 2 > max)
    {
        errno = EOVERFLOW;
        return -1;
    }
    written += 2;
    if (put('0') == EOF) { return -1; }
    if (put((acase == UPPER) ? 'X' : 'x') == EOF) { return -1; }

    if (trunc && dp == 0)
    {
        if (written + 3 > max)
        {
            errno = EOVERFLOW;
            return -1;
        }
        written += 3;
        if (put((f >= 1.5) ? '2' : '1') == EOF) { return -1; }
        int l = print_float_hex_exp(exp, put, written, max, acase);
        if (l == -1) { return -1; }
        written += l;
        return written;
    }

    if (written == max)
    {
        errno = EOVERFLOW;
        return -1;
    }
    written ++;
    if (put('1') == EOF) { return -1; }

    if (written == max)
    {
        errno = EOVERFLOW;
        return -1;
    }
    written ++;
    if (trunc || f != 1 || point) { if (put('.') == EOF) { return -1; } }

    f--;
    f *= 16;
    while ((!trunc && f != 0) || (trunc && dp > 0))
    {
        if (written == max)
        {
            errno = EOVERFLOW;
            return -1;
        }

        dp--;
        unsigned n = f;
        if (put((n < 10) ? n + '0' : n - 10 + 'A' + acase * 32) == EOF) { return -1; }
        written++;
        f -= n;
        f *= 16;
    }

    int l = print_float_hex_exp(exp, put, written, max, acase);
    if (l == -1) { return -1; }
    written += l;
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
            errno = EOVERFLOW;
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
        char flags = 0;
        switch (*format)
        {
            case '#':
                flags |= PRINTF_FLAG_ALT;
                format++;
                break;
            case '0':
                flags |= PRINTF_FLAG_ZERO;
                format++;
                break;
            case '-':
                flags |= PRINTF_FLAG_LEFT;
                format++;
                break;
            case ' ':
                flags |= PRINTF_FLAG_SPACE;
                format++;
                break;
            case '+':
                flags |= PRINTF_FLAG_SIGN;
                format++;
                break;
        }

        switch (*format)
        {
            int l;
            size_t len;
            case '%':
                format++;
                if (written == max)
                {
                    errno = EOVERFLOW;
                    return -1;
                }
                if(put('%') == EOF) { return -1; }
                written++;
                break;
            case 'c':
                format++;
                if (written == max)
                {
                    errno = EOVERFLOW;
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
                    errno = EOVERFLOW;
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
                        errno = EOVERFLOW;
                        return -1;
                    }
                    if(put('-') == EOF) { return -1; }
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

                if (o != 0 && (flags & PRINTF_FLAG_ALT))
                {
                    if (written == max)
                    {
                        errno = EOVERFLOW;
                        return -1;
                    }
                    if(put('0') == EOF) { return -1; }
                    written++;
                }

                if (o < 0)
                {
                    o = -o;
                    if (written == max)
                    {
                        errno = EOVERFLOW;
                        return -1;
                    }
                    if(put('-') == EOF) { return -1; }
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
                    errno = EOVERFLOW;
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
                if (flags & PRINTF_FLAG_ALT)
                {
                    if (written + 2 > max)
                    {
                        errno = EOVERFLOW;
                        return -1;
                    }

                    if(put('0') == EOF) { return -1; }
                    if(put('x') == EOF) { return -1; }
                }

                l = print_hex(x, put, written, max, LOWER);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'X':
                format++;
                unsigned X = va_arg(arg, unsigned);
                if (flags & PRINTF_FLAG_ALT)
                {
                    if (written + 2 > max)
                    {
                        errno = EOVERFLOW;
                        return -1;
                    }

                    if(put('0') == EOF) { return -1; }
                    if(put('X') == EOF) { return -1; }
                }

                l = print_hex(X, put, written, max, UPPER);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'p':
                format++;
                unsigned p = va_arg(arg, unsigned);
                if (written + 2 > max)
                {
                    errno = EOVERFLOW;
                    return -1;
                }

                if(put('0') == EOF) { return -1; }
                if(put('x') == EOF) { return -1; }
                l = print_hex(p, put, written, max, UPPER);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'f':
            case 'F':
                format++;
                double f = va_arg(arg, double);

                l = print_float(f, put, written, max, 6, false);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'e':
                format++;
                double e = va_arg(arg, double);

                l = print_exp(e, put, written, max, 6, !(flags & PRINTF_FLAG_ALT), LOWER);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'E':
                format++;
                double E = va_arg(arg, double);

                l = print_exp(E, put, written, max, 6, !(flags & PRINTF_FLAG_ALT), UPPER);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'g':
                format++;
                double g = va_arg(arg, double);

                double test_g = g;
                int exp_g = get_exp(&test_g, 10);
                if (exp_g < -4 || exp_g >= 6)
                {
                    l = print_exp(g, put, written, max, 5, !(flags & PRINTF_FLAG_ALT), LOWER);                    
                }
                else
                {
                    l = print_float(g, put, written, max, 5 - exp_g, !(flags & PRINTF_FLAG_ALT));
                }
                written += l;
                break;
            case 'G':
                format++;
                double G = va_arg(arg, double);

                double test_G = G;
                int exp_G = get_exp(&test_G, 10);
                if (exp_G < -4 || exp_G >= 6)
                {
                    l = print_exp(G, put, written, max, 5, !(flags & PRINTF_FLAG_ALT), UPPER);                    
                }
                else
                {
                    l = print_float(G, put, written, max, 5 - exp_G, !(flags & PRINTF_FLAG_ALT));
                }
                written += l;
                break;
            case 'a':
                format++;
                double a = va_arg(arg, double);

                l = print_float_hex(a, put, written, max, false, 0, flags & PRINTF_FLAG_ALT, LOWER);
                if (l == -1) { return -1; }
                written += l;
                break;
            case 'A':
                format++;
                double A = va_arg(arg, double);

                l = print_float_hex(A, put, written, max, false, 0, flags & PRINTF_FLAG_ALT, UPPER);
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
