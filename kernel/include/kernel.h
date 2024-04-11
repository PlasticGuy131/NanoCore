#ifndef KERNEL_H_INCLUDED
#define KERNEL_H_INCLUDED 1

#include <stddef.h>
#include <stdint.h>

extern uint32_t _multiboot_magic;
extern uint32_t _multiboot_info_start;

void kernel_alloc(size_t size);
void kernel_putchar(char c);
void kernel_panic(const char* error_message);

#endif