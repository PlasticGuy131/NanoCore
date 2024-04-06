#ifndef _STDIO_H
#define _STDIO_H 1

#define EOF (-1)

#include <stdarg.h>
#include <stddef.h>

int printf(const char* restrict format, ...);
int sprintf(char* s, const char* restrict format, ...);
int snprintf(char* s, const char* restrict format, size_t n, ...);
int vprintf(const char* restrict format, va_list arg);
int vsprintf(char* s, const char* restrict format, va_list arg);
int vsnprintf(char* s, const char* restrict format, size_t n, va_list arg);
void perror(const char* s);
int putchar(int ic);
int puts(const char* string);

#endif
