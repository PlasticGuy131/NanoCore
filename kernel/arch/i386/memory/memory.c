#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <kernel.h>
#include <multiboot.h>

extern uint32_t _heap_start;
extern uint32_t _heap_end;

typedef uint16_t bool8;

struct block_header
{
    uint16_t size;
    uint16_t prev;
    bool8 allocated;
};

static const uint8_t MEMORY_PENDING = 0xF0;

uint8_t* heap_start;

void memory_initialize()
{
    heap_start = (void*)(uintptr_t)_heap_start;
    struct block_header* header = (struct block_header*)heap_start;
    uint16_t heap_width = _heap_end - _heap_start;
    struct block_header initial_header;
    initial_header.size = heap_width - sizeof(struct block_header);
    initial_header.prev = 0;
    initial_header.allocated = false;
    *header = initial_header;
}

void* memory_alloc(size_t size)
{
    uint8_t* start = heap_start;
    struct block_header* header;
    bool found = false;
    while (!found)
    {
        header = (struct block_header*)start;

        if (header->size >= size && !header->allocated)
        {
            found = true;
            break;
        }

        if (header->size == 0) { return 0; }

        start += header->size;
    }

    uint16_t remainder = header->size - size;
    header->size = size;
    header->allocated = true;

    if(remainder <= sizeof(struct block_header))
    {
        uint8_t* new_start = start + size + sizeof(struct block_header);
        for (size_t i = 0; i < remainder; i++)
        {
            *new_start = MEMORY_PENDING;
        }
        return start;
    }
    uint8_t* new_start = start + size + sizeof(struct block_header);
    struct block_header new_header;
    new_header.size = remainder - sizeof(struct block_header);
    new_header.prev = size + sizeof(struct block_header);
    new_header.allocated = false;
    *(struct block_header*)new_start = new_header;
    return start;
}