#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void* realloc(void* ptr, size_t new_size)
{
    void* new_ptr = malloc(new_size);
    if (new_ptr == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    memcpy(new_ptr, ptr, new_size);
    free(ptr);
    return new_ptr;
}