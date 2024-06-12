#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef __is_libk
#include <kernel.h>
#endif

void* malloc(size_t size)
{
#ifdef __is_libk
    void *ptr = kernel_alloc(size);
    if (ptr == NULL) { errno = ENOMEM; }
    return ptr;
#endif
    return NULL;
}