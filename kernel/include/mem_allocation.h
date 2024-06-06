#ifndef MEM_ALLOCATOIN_H_INCLUDED
#define MEM_ALLOCATOIN_H_INCLUDED 1

#include <stddef.h>

/// @brief Allocates memory from the heap
/// @param size The size (in bytes) of memory to allocate.
/// @return A pointer to the allocated memory
void* memory_alloc(size_t size);

/// @brief Frees allocated memory
/// @param ptr Pointer to the allocated memory
void memory_free(void* ptr);

/// @brief Initializes the OS's memory
void memory_initialize();

#endif