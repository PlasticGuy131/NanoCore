#include <stdint.h>

#include <kernel.h>

struct GDT
{
    uint32_t limit;
    uint32_t base;
    uint8_t access_byte;
    uint8_t flags;
};

extern uint32_t gdt;

static void encode_GDT_entry(uint8_t* target, struct GDT source)
{
    if (source.limit > 0xFFFFF) { kernel_panic("GDT limit greater that 0xFFFFF"); }

    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] = (source.limit >> 16) & 0x0F;

    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;

    target[5] = source.access_byte;

    target[6] |= source.flags << 4;
}

void initialize_GDT()
{
    uint8_t* gdt_offset = (uint8_t*)gdt;

    struct GDT null;
    null.limit = 0;
    null.base = 0;
    null.access_byte = 0;
    null.flags = 0;
    encode_GDT_entry(gdt_offset, null);

    set_gdt();
}