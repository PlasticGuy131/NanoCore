#include <stddef.h>

#include "string.h"

void* memmove(void* dstptr, const void* srcptr, size_t size)
{
    unsigned char* dst = (unsigned char*)dstptr;
    const unsigned char* src = (const unsigned char*)srcptr;
    if (dst < src)
    {
        for (size_t i = 0; i < size; i++)
        {
            dst[i] = src[i];
        }
    }
    else if (src > dst)
    {
        for (size_t i = size-1; i > -1; i--)
        {
            dst[i] = src[i];
        }
    }
    return dstptr;
}