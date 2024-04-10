

#include <kernel.h>
#include <multiboot.h>


void memory_initalize(Multiboot_Info* multiboot_info)
{
    if (!(multiboot_info->flags & MULTIBOOT_FLAG_MMAP))
    {
        kernel_panic("No MMAP available.");
    }
}