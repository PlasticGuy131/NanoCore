#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <kernel.h>
#include <multiboot.h>

extern uint32_t _heap_start;
extern uint32_t _heap_end;

typedef uint16_t bool16;

struct block_header
{
    uint16_t size;
    bool16 allocated;
}

void memory_initialize()
{
    printf("Heap: %p - %p\n", _heap_start, _heap_end);
    struct block_header* header = (struct block_header*)(uintptr_t)_heap_start;
    uint16_t heap_width = _heap_end - _heap_start;
    struct block_header initial_header;
    initial_header.size = heap_width;
    initial_header.allocated = false;
    printf("Width: %p\n", heap_width);
}