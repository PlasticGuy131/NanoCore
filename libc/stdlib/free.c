#include <stddef.h>
#include <stdlib.h>

#ifdef __is_libk
#include <kernel.h>
#endif

void free(void* ptr)
{
#ifdef __is_libk
    kernel_free(ptr);
#endif
}