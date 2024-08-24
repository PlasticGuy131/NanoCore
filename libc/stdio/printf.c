#include <ctype.h>
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

enum Sign_Spacer
{
    NONE = 0,
    SPACE = ' ',
    PLUS = '+'
};

char* buf;
int offset;

char* widthBuffer;
unsigned widthUsage;
unsigned width;

static int wputchar(int ic)
{
    char c = (char)ic;
    if (widthUsage < width)
    {
        widthBuffer[widthUsage] = c;
        widthUsage++;
        return ic;
    }
    else { return EOF; }
}

static int get_exp(double* d, unsigned base)
{
    int exp = 0;
    if (*d == 0) { return 0; }
    while (*d >= base)
    {
        *d /= base;
        exp++;
    }
    while (*d < 1.0)
    {
        *d *= base;
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

static int print_float(double f, int (*put)(int), size_t written, unsigned max, unsigned dp, bool truncate, enum Sign_Spacer spacer, unsigned sig_digits)
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
    else if (spacer)
    {
        if (written == max)
        {
            errno = EOVERFLOW;
            return -1;
        }
        if(put(spacer == SPACE ? ' ' : '+') == EOF) { return -1; }
        written++;
    }
    unsigned integer = f;
    f -= integer;

    char* str = (char*)malloc((dp + 1) * sizeof(char));
    
    for (size_t i = 0; i < dp + 1; i++)
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

    if (sig_digits != 0)
    {
        bool significant = false;
        for (unsigned i = 0; i < dp + 1; i++)
        {
            if (str[i] != 0) { significant = true; }
            if (sig_digits && significant) { sig_digits--; }
            if (sig_digits == 0)
            {
                if (i != dp) { str[i + 1] = 10; }
                break;
            }
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

    if (truncate)
    {
        offset = dp;
        while (offset > 0)
        {
            offset--;
            if (str[offset] == 0 || str[offset] == 10) { str[offset] = 10; }
            else { break; }
        }

        if (offset == 0 && str[0] == 0)
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
        free(str);
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
}

static int print_exp(double f, int (*put)(int), size_t written, unsigned max, unsigned dp, bool truncate, enum Case ecase, enum Sign_Spacer spacer)
{
    int exp = get_exp(&f, 10);

    int l = print_float(f, put, written, max, dp, truncate, spacer, 0);
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
    if (exp < 10) { if (put('0') == EOF) { return -1; } }
    l = print_uint((unsigned)exp, put, written, max);
    if (l == -1) { return -1; }
    written += 4;
    return written;
}

static int print_float_hex_exp(int exp, int (*put)(int), size_t written, unsigned max, enum Case acase)
{
    if (put((acase == UPPER) ? 'P' : 'p') == EOF) { return -1; }
    if (put((exp >= 0) ? '+' : '-') == EOF) { return -1; }
    if (exp < 0) { exp *= -1; }
    int l = print_uint(exp, put, written, max);
    if (l == -1) { return -1; }
    written += l;
    return written;
}

static int print_float_hex(double f, int (*put)(int), size_t written, unsigned max, bool defaultPrecision, unsigned dp, bool point, enum Case acase, enum Sign_Spacer spacer)
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
    else if (spacer)
    {
        if (written == max)
        {
            errno = EOVERFLOW;
            return -1;
        }
        if(put(spacer == SPACE ? ' ' : '+') == EOF) { return -1; }
        written++;
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

    if (f == 0)
    {
        if (written == max)
        {
            errno = EOVERFLOW;
            return -1;
        }
        if (put('0') == EOF) { return -1; }
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
    if (put('1') == EOF) { return -1; }

    if (defaultPrecision) { dp = 13; }
    unsigned char* str = (unsigned char*)malloc((dp + 1) * sizeof(unsigned char));

    f--;
    f *= 16;

    for (unsigned i = 0; i < dp + 1; i++)
    {
        unsigned n = f;
        str[i] = n;
        f -= n;
        f *= 16;
    }

    if (str[dp] > 8) { str[dp - 1]++; }
    for (unsigned i = 0; i < dp; i++)
    {
        if (str[dp - i] == 16) { str[dp - i - 1]++; }
        else { break; }
    }

    unsigned offset = dp + 1;
    for (unsigned i = 0; i < dp + 1; i++)
    {
        offset--;
        if (str[offset] == 0) { str[offset] = 16; }
        else { break; }
    }

    if (offset != 0 || point)
    {
        if (put('.') == EOF)
        {
            free(str);
            return -1;
        }
    }

    if (offset != 0)
    {
        for (unsigned i = 0; i < dp; i++)
        {
            if (str[i] == 16) { break; }
            if (written == max)
            {
                errno = EOVERFLOW;
                free(str);
                return -1;
            }
            if (put((str[i] < 10) ? str[i] + (unsigned)'0' : str[i] - 10 + 'A' + acase * 32) == EOF)
            {
                free(str);
                return -1;
            }
        }
    }
    free(str);

    int l = print_float_hex_exp(exp, put, written, max, acase);
    if (l == -1) { return -1; }
    written += l;
    return written;
}

static int n_arg(va_list arg, unsigned n)
{
    va_list copy;
    va_copy(copy, arg);
    for (unsigned i = 0; i < n - 1; i++) { va_arg(copy, int); }
    return va_arg(copy, int);
}

static int vaprintf(const char* restrict format, int (*put)(int), unsigned max, va_list arg)
{
    va_list vaOriginal;
    va_copy(vaOriginal, arg);
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
        bool onFlags = true;
        while (onFlags)
        {
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
            default:
                onFlags = false;
                break;
            }
        }
        enum Sign_Spacer spacer = NONE;
        if (flags & PRINTF_FLAG_SPACE) { spacer = SPACE; }
        if (flags & PRINTF_FLAG_SIGN) { spacer = PLUS; }

        bool hasWidth = false;
        int (*realPut)(int);
        va_list vaBackup;
        if (isdigit(*format))
        {
            hasWidth = true;
            width = atoi(format);
            while (isdigit(*format)) { format++; }
        }

        if (*format == '*')
        {
            hasWidth = true;
            format++;
            if (isdigit(*format))
            {
                unsigned n = atoi(format);
                while (isdigit(*format)) { format++; }
                if (*format != '$') { return -1; }
                format++;
                width = n_arg(vaOriginal, n);
            }
            else { width = va_arg(arg, int); }
        }

        bool hasPrecision = false;
        unsigned precision = 0;
        if (*format == '.')
        {
            hasPrecision = true;
            format++;
            if (isdigit(*format))
            {
                precision = atoi(format);
                while (isdigit(*format)) { format++; }
            }
            else if (*format == '*')
            {
                format++;
                if (isdigit(*format))
                {
                    unsigned n = atoi(format);
                    while (isdigit(*format)) { format++; }
                    if (*format != '$') { return -1; }
                    format++;
                    precision = n_arg(vaOriginal, n);
                }
                else { precision = va_arg(arg, int); }
            }
        }

        if (hasWidth)
        {
            widthBuffer = (char*)malloc(width * sizeof(char));
            widthUsage = 0;
            realPut = put;
            put = &wputchar;
            va_copy(vaBackup, arg);
        }

        bool isNumeric = false;
        unsigned passes = 0;
        unsigned maxPasses = 1;
        while (passes < maxPasses)
        {
            passes++;
            enum Case printCase = LOWER;
            bool didEOF = false;
            switch (*format)
            {
                int l;
                size_t len;
            case '%':
                if (written == max)
                {
                    errno = EOVERFLOW;
                    return -1;
                }
                if (put('%') == EOF) { return -1; }
                written++;
                break;
            case 'c':
                if (written == max)
                {
                    errno = EOVERFLOW;
                    return -1;
                }
                int c = va_arg(arg, int);
                if (put(c) == EOF) { return -1; }
                written++;
                break;
            case 's':
                const char* s = va_arg(arg, const char*);
                len = strlen(s);
                if (hasPrecision && precision < len) { len = precision; }
                if (written + len > max)
                {
                    errno = EOVERFLOW;
                    return -1;
                }
                for (size_t i = 0; i < len; i++)
                {
                    if (put(s[i]) == EOF)
                    {
                        didEOF = true;
                        break;
                    }
                }
                written += len;
                break;
            case 'd':
            case 'i':
                isNumeric = true;
                int i = va_arg(arg, int);

                if (i < 0)
                {
                    i = -i;
                    if (written == max)
                    {
                        errno = EOVERFLOW;
                        return -1;
                    }
                    if (put('-') == EOF) 
                    {
                        didEOF = true;
                        break;
                    }
                    written++;
                }
                else if (spacer)
                {
                    if (written == max)
                    {
                        errno = EOVERFLOW;
                        return -1;
                    }
                    if (put(spacer == SPACE ? ' ' : '+') == EOF)
                    {
                        didEOF = true;
                        break;
                    }
                    written++;
                }

                if (hasPrecision)
                {
                    double test_i = i;
                    unsigned digits = get_exp(&test_i, 10) + 1;
                    if (digits < precision)
                    {
                        for (unsigned j = 0; j < precision - digits; j++)
                        {
                            if (put('0') == EOF)
                            {
                                didEOF = true;
                                break;
                            }
                        }
                    }
                }

                l = print_uint(i, put, written, max);
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    didEOF = true;
                    break;
                }
                written += l;
                break;
            case 'u':
                isNumeric = true;
                int u = va_arg(arg, int);

                if (hasPrecision)
                {
                    double test_u = u;
                    unsigned digits = get_exp(&test_u, 10)  + 1;
                    if (digits < precision)
                    {
                        for (unsigned j = 0; j < precision - digits; j++)
                        {
                            if (put('0') == EOF)
                            {
                                didEOF = true;
                                break;
                            }
                        }
                    }
                }
                
                l = print_uint(u, put, written, max);
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    didEOF = true;
                    break;
                }
                written += l;
                break;
            case 'o':
                isNumeric = true;
                int o = va_arg(arg, int);

                if (o != 0 && (flags & PRINTF_FLAG_ALT))
                {
                    precision--;
                    if (written == max)
                    {
                        errno = EOVERFLOW;
                        return -1;
                    }
                    if (put('0') == EOF)
                    {
                        didEOF = true;
                        break;
                    }
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
                    if (put('-') == EOF)
                    {
                        didEOF = true;
                        break;
                    }
                    written++;
                }

                if (hasPrecision)
                {
                    double test_o = o;
                    unsigned digits = get_exp(&test_o, 8) + 1;
                    if (digits < precision)
                    {
                        for (unsigned j = 0; j < precision - digits; j++)
                        {
                            if (put('0') == EOF)
                            {
                                didEOF = true;
                                break;
                            }
                        }
                    }
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
                    if (put(ch + '0') == EOF)
                    {
                        didEOF = true;
                        break;
                    }
                    o %= header;
                    header /= 8;
                }
                written += len;
                break;
            case 'X':
                printCase = UPPER;
                [[fallthrough]];
            case 'x':
                isNumeric = true;
                unsigned x = va_arg(arg, unsigned);
                if (flags & PRINTF_FLAG_ALT)
                {
                    if (written + 2 > max)
                    {
                        errno = EOVERFLOW;
                        return -1;
                    }

                    if (put('0') == EOF)
                    {
                        didEOF = true;
                        break;
                    }
                    if (put('x') == EOF)
                    {
                        didEOF = true;
                        break;
                    }
                }

                if (hasPrecision)
                {
                    double test_x = x;
                    unsigned digits = get_exp(&test_x, 16) + 1;
                    if (digits < precision)
                    {
                        for (unsigned j = 0; j < precision - digits; j++)
                        {
                            if (put('0') == EOF)
                            {
                                didEOF = true;
                                break;
                            }
                        }
                    }
                }

                l = print_hex(x, put, written, max, printCase);
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    didEOF = true;
                    break;
                }
                written += l;
                break;
            case 'p':
                unsigned p = va_arg(arg, unsigned);
                if (written + 2 > max)
                {
                    errno = EOVERFLOW;
                    return -1;
                }

                if (put('0') == EOF)
                {
                    didEOF = true;
                    break;
                }
                if (put('x') == EOF)
                {
                    didEOF = true;
                    break;
                }
                l = print_hex(p, put, written, max, UPPER);
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    didEOF = true;
                    break;
                }
                written += l;
                break;
            case 'f':
            case 'F':
                isNumeric = true;
                double f = va_arg(arg, double);

                l = print_float(f, put, written, max, hasPrecision ? precision : 6, false, spacer, 0);
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    didEOF = true;
                    break;
                }
                written += l;
                break;
            case 'E':
                printCase = UPPER;
                [[fallthrough]];
            case 'e':
                isNumeric = true;
                double e = va_arg(arg, double);

                l = print_exp(e, put, written, max, hasPrecision ? precision : 6, !(flags & PRINTF_FLAG_ALT), printCase, spacer);
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    didEOF = true;
                    break;
                }
                written += l;
                break;
            case 'G':
                printCase = UPPER;
                [[fallthrough]];
            case 'g':
                isNumeric = true;
                double g = va_arg(arg, double);

                double test_g = g;
                int exp_g = get_exp(&test_g, 10);

                int g_precision = hasPrecision ? precision : 6;
                if (exp_g < -4 || exp_g >= g_precision)
                {
                    l = print_exp(g, put, written, max, g_precision - 1, !(flags & PRINTF_FLAG_ALT), printCase, spacer);
                }
                else
                {
                    l = print_float(g, put, written, max, 5 - exp_g, !(flags & PRINTF_FLAG_ALT), spacer, hasPrecision ? precision : 0);
                }
                
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    didEOF = true;
                    break;
                }
                written += l;
                break;
            case 'A':
                printCase = UPPER;
                [[fallthrough]];
            case 'a':
                isNumeric = true;
                double a = va_arg(arg, double);

                l = print_float_hex(a, put, written, max, !hasPrecision, precision, flags & PRINTF_FLAG_ALT, printCase, spacer);
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    didEOF = true;
                    break;
                }
                written += l;
                break;
            case 'n':
                int* n = va_arg(arg, int*);
                *n = written;
            }

            if (didEOF)
            {
                if (hasWidth)
                {
                    arg = vaBackup;
                    put = realPut;
                    maxPasses = 2;
                }
                else { return -1; }
            }
        }

        if (hasWidth)
        {
            put = realPut;
            if (passes == 1)
            {
                if (!(flags & PRINTF_FLAG_LEFT))
                {
                    for (unsigned i = 0; i < width - widthUsage; i++)
                    {
                        if (put((flags & PRINTF_FLAG_ZERO) && isNumeric ? '0' : ' ') == EOF) { return -1; }
                    }
                }
                for (unsigned i = 0; i < widthUsage; i++)
                {
                    if (put(widthBuffer[i]) == EOF) { return -1; }
                }
                if (flags & PRINTF_FLAG_LEFT)
                {
                    for (unsigned i = 0; i < width - widthUsage; i++)
                    {
                        if (put(' ') == EOF) { return -1; }
                    }
                }
            }
            free(widthBuffer);
        }
        format++;
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
