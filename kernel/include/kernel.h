#ifndef KERNEL_H_INCLUDED
#define KERNEL_H_INCLUDED 1

#include <stdint.h>

extern uint32_t multiboot_magic;
extern uint32_t multiboot_info_start;

void kernel_putchar(char c);

#endif