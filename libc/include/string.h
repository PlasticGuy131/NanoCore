#ifndef _STRING_H
#define _STRING_H 1

#include <stddef.h>

int memcmp(const void *aptr, const void *bptr, size_t size);
void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size);
void* memmove(void* dstptr, const void* srcptr, size_t size);
void* memset(void *bufptr, int value, size_t size);
char* strerror(int errnum);
char* strcpy(char* dest, const char* src);
size_t strlen(const char* str);

#endif