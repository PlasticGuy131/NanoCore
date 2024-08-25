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

enum Int_Width
{
    DEFAULT,
    CHAR,
    SHORT,
    LONG,
    LONG_LONG,
    MAX,
    SIZE,
    PTR
};

char* buf;
int offset;

char* width_buffer;
unsigned width_usage;
unsigned width;

static int wputchar(int ic)
{
    char c = (char)ic;
    if (width_usage < width)
    {
        width_buffer[width_usage] = c;
        width_usage++;
        return ic;
    }
    else { return EOF; }
}

static int get_exp(long double* d, unsigned base)
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

static int print_uint(long long unsigned i, int (*put)(int), size_t written, unsigned max)
{
    size_t len = 1;
    long long unsigned header = 1;
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

static int print_hex(long long unsigned i, int (*put)(int), size_t written, unsigned max, enum Case hcase)
{
    size_t len = 1;
    long long unsigned header = 1;
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

static int print_float(long double f, int (*put)(int), size_t written, unsigned max, unsigned dp, bool truncate, enum Sign_Spacer spacer, unsigned sig_digits)
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

        if (str[0] == 10)
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

static int print_exp(long double f, int (*put)(int), size_t written, unsigned max, unsigned dp, bool truncate, enum Case ecase, enum Sign_Spacer spacer)
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

static int print_float_hex(long double f, int (*put)(int), size_t written, unsigned max, bool default_precision, unsigned dp, bool point, enum Case acase, enum Sign_Spacer spacer)
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

    if (default_precision) { dp = 13; }
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
    va_list va_original;
    va_copy(va_original, arg);
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

        bool has_width = false;
        int (*real_put)(int);
        va_list va_backup;
        if (isdigit(*format))
        {
            has_width = true;
            width = atoi(format);
            while (isdigit(*format)) { format++; }
        }

        if (*format == '*')
        {
            has_width = true;
            format++;
            if (isdigit(*format))
            {
                unsigned n = atoi(format);
                while (isdigit(*format)) { format++; }
                if (*format != '$') { return -1; }
                format++;
                width = n_arg(va_original, n);
            }
            else { width = va_arg(arg, int); }
        }

        if (has_width)
        {
            width_buffer = (char*)malloc(width * sizeof(char));
            width_usage = 0;
            real_put = put;
            put = &wputchar;
            va_copy(va_backup, arg);
        }

        bool has_precision = false;
        unsigned precision = 0;
        if (*format == '.')
        {
            has_precision = true;
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
                    precision = n_arg(va_original, n);
                }
                else { precision = va_arg(arg, int); }
            }
        }

        bool long_double = false;
        enum Int_Width int_width = DEFAULT;
        switch (*format)
        {
        case 'h':
            format++;
            if (*format == 'h')
            {
                format++;
                int_width = CHAR;
            }
            else { int_width = SHORT; }
            break;
        case 'l':
            format++;
            if (*format == 'l')
            {
                format++;
                int_width = LONG_LONG;
            }
            else { int_width = LONG; }
            break;
        case 'j':
            format++;
            int_width = MAX;
            break;
        case 'z':
            format++;
            int_width = SIZE;
            break;
        case 't':
            format++;
            int_width = PTR;
            break;
        case 'L':
            format++;
            long_double = true;
            break;
        }

        bool is_numeric = false;
        unsigned passes = 0;
        unsigned max_passes = 1;
        while (passes < max_passes)
        {
            passes++;
            enum Case print_case = LOWER;
            bool did_EOF = false;
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
                if (has_precision && precision < len) { len = precision; }
                if (written + len > max)
                {
                    errno = EOVERFLOW;
                    return -1;
                }
                for (size_t i = 0; i < len; i++)
                {
                    if (put(s[i]) == EOF)
                    {
                        did_EOF = true;
                        break;
                    }
                }
                written += len;
                break;
            case 'd':
            case 'i':
                is_numeric = true;
                long long int i;
                switch (int_width)
                {
                case DEFAULT:
                    i = va_arg(arg, int);
                    break;
                case CHAR:
                    i = (char)va_arg(arg, int);
                    break;
                case SHORT:
                    i = (short int)va_arg(arg, int);
                    break;
                case LONG:
                    i = va_arg(arg, long int);
                    break;
                case LONG_LONG:
                    i = va_arg(arg, long long int);
                    break;
                case MAX:
                    i = va_arg(arg, intmax_t);
                    break;
                case SIZE:
                    i = va_arg(arg, size_t);
                    break;
                case PTR:
                    i = va_arg(arg, ptrdiff_t);
                    break;
                }

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
                        did_EOF = true;
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
                        did_EOF = true;
                        break;
                    }
                    written++;
                }

                if (has_precision)
                {
                    long double test_i = i;
                    unsigned digits = get_exp(&test_i, 10) + 1;
                    if (digits < precision)
                    {
                        for (unsigned j = 0; j < precision - digits; j++)
                        {
                            if (put('0') == EOF)
                            {
                                did_EOF = true;
                                break;
                            }
                        }
                    }
                }

                l = print_uint(i, put, written, max);
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    did_EOF = true;
                    break;
                }
                written += l;
                break;
            case 'u':
                is_numeric = true;
                long long unsigned u;
                switch (int_width)
                {
                case DEFAULT:
                    u = va_arg(arg, unsigned);
                    break;
                case CHAR:
                    u = (unsigned char)va_arg(arg, unsigned);
                    break;
                case SHORT:
                    u = (short unsigned)va_arg(arg, unsigned);
                    break;
                case LONG:
                    u = va_arg(arg, long unsigned);
                    break;
                case LONG_LONG:
                    u = va_arg(arg, long long unsigned);
                    break;
                case MAX:
                    u = va_arg(arg, uintmax_t);
                    break;
                case SIZE:
                    u = va_arg(arg, size_t);
                    break;
                case PTR:
                    u = va_arg(arg, ptrdiff_t);
                    break;
                }

                if (has_precision)
                {
                    long double test_u = u;
                    unsigned digits = get_exp(&test_u, 10)  + 1;
                    if (digits < precision)
                    {
                        for (unsigned j = 0; j < precision - digits; j++)
                        {
                            if (put('0') == EOF)
                            {
                                did_EOF = true;
                                break;
                            }
                        }
                    }
                }
                
                l = print_uint(u, put, written, max);
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    did_EOF = true;
                    break;
                }
                written += l;
                break;
            case 'o':
                is_numeric = true;
                long long unsigned o;
                switch (int_width)
                {
                case DEFAULT:
                    o = va_arg(arg, unsigned);
                    break;
                case CHAR:
                    o = (unsigned char)va_arg(arg, unsigned);
                    break;
                case SHORT:
                    o = (short unsigned)va_arg(arg, unsigned);
                    break;
                case LONG:
                    o = va_arg(arg, long unsigned);
                    break;
                case LONG_LONG:
                    o = va_arg(arg, long long unsigned);
                    break;
                case MAX:
                    o = va_arg(arg, uintmax_t);
                    break;
                case SIZE:
                    o = va_arg(arg, size_t);
                    break;
                case PTR:
                    o = va_arg(arg, ptrdiff_t);
                    break;
                }

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
                        did_EOF = true;
                        break;
                    }
                    written++;
                }

                if (has_precision)
                {
                    long double test_o = o;
                    unsigned digits = get_exp(&test_o, 8) + 1;
                    if (digits < precision)
                    {
                        for (unsigned j = 0; j < precision - digits; j++)
                        {
                            if (put('0') == EOF)
                            {
                                did_EOF = true;
                                break;
                            }
                        }
                    }
                }

                len = 1;
                long long unsigned header = 1;
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
                        did_EOF = true;
                        break;
                    }
                    o %= header;
                    header /= 8;
                }
                written += len;
                break;
            case 'X':
                print_case = UPPER;
                [[fallthrough]];
            case 'x':
                is_numeric = true;
                long long unsigned x;
                switch (int_width)
                {
                case DEFAULT:
                    x = va_arg(arg, unsigned);
                    break;
                case CHAR:
                    x = (unsigned char)va_arg(arg, unsigned);
                    break;
                case SHORT:
                    x = (short unsigned)va_arg(arg, unsigned);
                    break;
                case LONG:
                    x = va_arg(arg, long unsigned);
                    break;
                case LONG_LONG:
                    x = va_arg(arg, long long unsigned);
                    break;
                case MAX:
                    x = va_arg(arg, uintmax_t);
                    break;
                case SIZE:
                    x = va_arg(arg, size_t);
                    break;
                case PTR:
                    x = va_arg(arg, ptrdiff_t);
                    break;
                }

                if (flags & PRINTF_FLAG_ALT)
                {
                    if (written + 2 > max)
                    {
                        errno = EOVERFLOW;
                        return -1;
                    }

                    if (put('0') == EOF)
                    {
                        did_EOF = true;
                        break;
                    }
                    if (put('x') == EOF)
                    {
                        did_EOF = true;
                        break;
                    }
                }

                if (has_precision)
                {
                    long double test_x = x;
                    unsigned digits = get_exp(&test_x, 16) + 1;
                    if (digits < precision)
                    {
                        for (unsigned j = 0; j < precision - digits; j++)
                        {
                            if (put('0') == EOF)
                            {
                                did_EOF = true;
                                break;
                            }
                        }
                    }
                }

                l = print_hex(x, put, written, max, print_case);
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    did_EOF = true;
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
                    did_EOF = true;
                    break;
                }
                if (put('x') == EOF)
                {
                    did_EOF = true;
                    break;
                }
                l = print_hex(p, put, written, max, UPPER);
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    did_EOF = true;
                    break;
                }
                written += l;
                break;
            case 'f':
            case 'F':
                is_numeric = true;
                long double f;
                if (long_double) { f = va_arg(arg, long double); }
                else { f = va_arg(arg, double); }

                l = print_float(f, put, written, max, has_precision ? precision : 6, false, spacer, 0);
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    did_EOF = true;
                    break;
                }
                written += l;
                break;
            case 'E':
                print_case = UPPER;
                [[fallthrough]];
            case 'e':
                is_numeric = true;
                long double e;
                if (long_double) { e = va_arg(arg, long double); }
                else { e = va_arg(arg, double); }

                l = print_exp(e, put, written, max, has_precision ? precision : 6, !(flags & PRINTF_FLAG_ALT), print_case, spacer);
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    did_EOF = true;
                    break;
                }
                written += l;
                break;
            case 'G':
                print_case = UPPER;
                [[fallthrough]];
            case 'g':
                is_numeric = true;
                long double g;
                if (long_double) { g = va_arg(arg, long double); }
                else { g = va_arg(arg, double); }

                long double test_g = g;
                int exp_g = get_exp(&test_g, 10);

                int g_precision = has_precision ? precision : 6;
                if (exp_g < -4 || exp_g >= g_precision)
                {
                    l = print_exp(g, put, written, max, g_precision - 1, !(flags & PRINTF_FLAG_ALT), print_case, spacer);
                }
                else
                {
                    l = print_float(g, put, written, max, 5 - exp_g, !(flags & PRINTF_FLAG_ALT), spacer, has_precision ? precision : 0);
                }
                
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    did_EOF = true;
                    break;
                }
                written += l;
                break;
            case 'A':
                print_case = UPPER;
                [[fallthrough]];
            case 'a':
                is_numeric = true;
                long double a;
                if (long_double) { a = va_arg(arg, long double); }
                else { a = va_arg(arg, double); }

                l = print_float_hex(a, put, written, max, !has_precision, precision, flags & PRINTF_FLAG_ALT, print_case, spacer);
                if (l == -1)
                {
                    if (errno == EOVERFLOW) { return -1; }
                    did_EOF = true;
                    break;
                }
                written += l;
                break;
            case 'n':
                switch (int_width)
                {
                case DEFAULT:
                    int* n = va_arg(arg, int*);
                    *n = written;
                    break;
                case CHAR:
                    char* hhn = va_arg(arg, char*);
                    *hhn = written;
                    break;
                case SHORT:
                    short int* hn = va_arg(arg, short int*);
                    *hn = written;
                    break;
                case LONG:
                    long int* ln = va_arg(arg, long int*);
                    *ln = written;
                    break;
                case LONG_LONG:
                    long long int* lln = va_arg(arg, long long int*);
                    *lln = written;
                    break;
                case MAX:
                    intmax_t* jn = va_arg(arg, intmax_t*);
                    *jn = written;
                    break;
                case SIZE:
                    size_t* zn = va_arg(arg, size_t*);
                    *zn = written;
                    break;
                case PTR:
                    ptrdiff_t* tn = va_arg(arg, ptrdiff_t*);
                    *tn = written;
                    break;
                }
                
            }

            if (did_EOF)
            {
                if (has_width)
                {
                    arg = va_backup;
                    put = real_put;
                    max_passes = 2;
                }
                else { return -1; }
            }
        }

        if (has_width)
        {
            put = real_put;
            if (passes == 1)
            {
                if (!(flags & PRINTF_FLAG_LEFT))
                {
                    for (unsigned i = 0; i < width - width_usage; i++)
                    {
                        if (put((flags & PRINTF_FLAG_ZERO) && is_numeric ? '0' : ' ') == EOF) { return -1; }
                    }
                }
                for (unsigned i = 0; i < width_usage; i++)
                {
                    if (put(width_buffer[i]) == EOF) { return -1; }
                }
                if (flags & PRINTF_FLAG_LEFT)
                {
                    for (unsigned i = 0; i < width - width_usage; i++)
                    {
                        if (put(' ') == EOF) { return -1; }
                    }
                }
            }
            free(width_buffer);
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
