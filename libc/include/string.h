#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED 1

#include <stddef.h>

int memcmp(const void *aptr, const void *bptr, size_t size);
void* memcpy(restrict void* dstptr, const restrict void* srcptr, size_t size);
void* memmove(void* dstptr, const void* srcptr, size_t size);
void* memset(void *bufptr, int value, size_t size);
size_t strlen(const char* str);

#endif