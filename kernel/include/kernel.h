#ifndef KERNEL_H_INCLUDED
#define KERNEL_H_INCLUDED 1

#include <stddef.h>
#include <stdint.h>

extern uint32_t _multiboot_magic;
extern uint32_t _multiboot_info_start;

/// @brief Allocates memory from the heap
/// @param size The size (in bytes) of memory to allocate.
/// @return A pointer to the allocated memory
void* kernel_alloc(size_t size);

/// @brief Frees allocated memory
/// @param ptr Pointer to the allocated memory
void kernel_free(void* ptr);

/// @brief Writes a character to the screen
/// @param c Character to write
void kernel_putchar(unsigned char c);

/// @brief Halts the system, CALL ONLY IN DISASTER!
/// @param error_message Error message to be displayed
void kernel_panic(const char* error_message);

#endif