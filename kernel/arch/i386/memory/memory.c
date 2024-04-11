#include <stdint.h>
#include <stdio.h>

#include <kernel.h>
#include <multiboot.h>

extern uint32_t _heap_start;
extern uint32_t _heap_end;

void memory_initialize(Multiboot_Info* multiboot_info)
{
    printf("Heap: %p - %p\n", _heap_start, _heap_end);
    uint32_t* heap_start = (uint32_t*)(uintptr_t)_heap_start;
    uint32_t heap_width = _heap_end - _heap_start;
    *heap_start = heap_width;
    printf("Width: %p\n", heap_width);
}