#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <kernel.h>
#include <multiboot.h>

extern uint32_t _heap_start;
extern uint32_t _heap_end;

typedef uint8_t bool8;

struct block_header
{
    uint16_t size;
    uint16_t prev;
    bool8 allocated;
};

static const uint8_t MEMORY_PENDING = 0;
static const size_t HEADER_WIDTH = sizeof(struct block_header);

uint8_t* heap_start;

int memory_usage()
{
    uint32_t usage = 0;
    uint8_t* start = heap_start;
    struct block_header* header = (struct block_header*)heap_start;
    while ((uint32_t)start + header->size + HEADER_WIDTH < _heap_end)
    {
        if (header->allocated) { usage += header->size; };
        start += header->size + HEADER_WIDTH;
        while (*start == MEMORY_PENDING) { start++; }
    }
    return usage;
}

int memory_max() { return _heap_end - _heap_start; }

void memory_initialize()
{
    heap_start = (void*)(uintptr_t)_heap_start;
    struct block_header* header = (struct block_header*)heap_start;
    uint16_t heap_width = _heap_end - _heap_start;
    struct block_header initial_header;
    initial_header.size = heap_width - HEADER_WIDTH;
    initial_header.prev = 0;
    initial_header.allocated = false;
    *header = initial_header;
}

void* memory_alloc(size_t size)
{
    if (size == 0) { return 0; }
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

        if ((uint32_t)start + header->size + HEADER_WIDTH >= _heap_end) { return 0; }

        start += header->size + HEADER_WIDTH;
        while (*start == MEMORY_PENDING) { start++; }
    }

    uint16_t remainder = header->size - size;
    header->size = size;
    header->allocated = true;

    if(remainder <= HEADER_WIDTH)
    {
        uint8_t* new_start = start + size + HEADER_WIDTH;
        for (size_t i = 0; i < remainder; i++)
        {
            *new_start = MEMORY_PENDING;
        }
        return start;
    }
    uint8_t* new_start = start + size + HEADER_WIDTH;
    struct block_header new_header;
    new_header.size = remainder - HEADER_WIDTH;
    new_header.prev = size + HEADER_WIDTH;
    new_header.allocated = false;
    *(struct block_header*)new_start = new_header;

    uint8_t* next_ptr = new_start + new_header.size + HEADER_WIDTH;
    if ((uint32_t)next_ptr >= _heap_end) { return start; }
    struct block_header* next_header = (struct block_header*)next_ptr;
    next_header->prev = (uint16_t)(uintptr_t)next_ptr - (uint16_t)(uintptr_t)new_start;

    return start;
}

void memory_free(uint8_t* ptr)
{
    struct block_header* header = (struct block_header*)ptr;
    header->allocated = false;
    while ((size_t)ptr + header->size + HEADER_WIDTH < _heap_end)
    {
        uint8_t* next_ptr = ptr;
        next_ptr += header->size + HEADER_WIDTH;
        while (*next_ptr == MEMORY_PENDING)
        {
            header->size++;
            next_ptr++;
        }

        struct block_header* next_header = (struct block_header*)next_ptr;
        if (next_header->allocated) { break; }

        header->size += next_header->size + HEADER_WIDTH;
        if ((size_t)ptr + header->size + HEADER_WIDTH < _heap_end)
        {
            next_ptr = ptr + header->size + HEADER_WIDTH;
            next_header = (struct block_header*)next_ptr;
            next_header->prev = (uint16_t)(uintptr_t)next_ptr - (uint16_t)(uintptr_t)ptr;
        }
    }

    while (header->prev != 0)
    {
        uint8_t* prev_ptr = ptr - header->prev;

        struct block_header* prev_header = (struct block_header*)prev_ptr;

        uint8_t* end = prev_ptr + prev_header->size + HEADER_WIDTH;

        if (end != ptr)
        {
            memmove(end, ptr, HEADER_WIDTH);
            size_t gaps = (size_t)ptr - (size_t)end; 
            ptr = end;
            header = (struct block_header*)ptr;
            header->size += gaps;
            header->prev -= gaps;
        }
      
        if (prev_header->allocated) { break; }

        prev_header->size += header->size + HEADER_WIDTH;
        if ((size_t)ptr + header->size < _heap_end)
        {
            uint8_t* next_ptr = ptr + header->size + HEADER_WIDTH;
            struct block_header* next_header = (struct block_header*)next_ptr;
            next_header->prev = (uint16_t)(uintptr_t)next_ptr - (uint16_t)(uintptr_t)prev_ptr;
        }
        header = prev_header;
    }
}

void memory_visualise()
{
    uint8_t* start = heap_start;
    struct block_header* header = (struct block_header*)heap_start;
    while ((uint32_t)start < _heap_end)
    {
        printf("H");
        if (header->allocated) { printf("#%i#", header->size); }
        else { printf("O%iO", header->size); }

        start += header->size + HEADER_WIDTH;
        while (*start == MEMORY_PENDING)
        {
            printf("|");
            start++;
        }
    }
}