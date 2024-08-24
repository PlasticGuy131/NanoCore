#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <stddef.h>

void* malloc(size_t size);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t new_size);

void free(void* ptr);

int atoi(const char* str);

#endif