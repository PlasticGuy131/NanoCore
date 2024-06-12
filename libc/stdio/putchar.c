#include <stdio.h>

#ifdef __is_libk
#include <kernel.h>
#endif

int putchar(int ic)
{
#ifdef __is_libk
    char c = (char)ic;
    kernel_putchar(c);
#else
    //TODO: system calls..
#endif
    return ic;
}