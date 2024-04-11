#include <stdint.h>
#include <stdio.h>

#include <kernel.h>
#include <multiboot.h>

static const uint8_t MMAP_TYPE_RAM = 1;

struct mmap
{
    uint32_t size;
    uint64_t base_addr;
    uint64_t length;
    uint8_t  type;
};

void memory_initialize(Multiboot_Info* multiboot_info)
{
    if (!(multiboot_info->flags & MULTIBOOT_FLAG_MMAP))
    {
        kernel_panic("No MMAPs available."); // Might not actually have to panic idk...
    }

    printf("MMAP length: %d\n", multiboot_info->mmap_length);
    struct mmap* mmap = (struct mmap*)(multiboot_info->mmap_addr - 4);
    for (size_t i = 0; i < multiboot_info->mmap_length; i++)
    {
        printf("SIZE: %d\n", mmap->size);
        printf("BASE ADDR: %d\n", mmap->base_addr);
        printf("LENGTH: %d\n", mmap->length);
        printf("TYPE: %d\n", mmap->type);
        void* v_mmap = (void*)mmap;
        v_mmap += mmap->size;
        mmap = (struct mmap *)v_mmap;
    }
}