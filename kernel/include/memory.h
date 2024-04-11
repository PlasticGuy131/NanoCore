#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED 1

#include <stddef.h>

void* memory_alloc(size_t size);
void memory_initialize();

#endif