#ifndef _STDIO_H
#define _STDIO_H 1

#define EOF (-1)

#include <stdarg.h>

int printf(const char* restrict format, ...);
int sprintf(const char* restrict format, ...);
int snprintf(const char* restrict format, ...);
int vprintf(const char* restrict format, va_list arg);
int vsprintf(const char* restrict format, va_list arg);
int vsnprintf(const char* restrict format, va_list arg);
int putchar(int ic);
int puts(const char* string);

#endif
