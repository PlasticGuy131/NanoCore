#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void* calloc(size_t num, size_t size)
{
    if (size == 0) { return NULL; }
    uint8_t* ptr = (uint8_t*)malloc(num * size);
    for (size_t i = 0; i < num * size; i++)
    {
        *ptr = 0;
    }
    return (void*)ptr;
}