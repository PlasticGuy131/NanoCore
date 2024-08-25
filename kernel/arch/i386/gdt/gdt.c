#include <stdint.h>
#include <stdio.h>

#include <kernel.h>

static const uint8_t ACCESS_BYTE_ACCESSED = 1;
static const uint8_t ACCESS_BYTE_CODE_READ = 1 << 1;
static const uint8_t ACCESS_BYTE_DATA_WRITE = 1 << 1;
static const uint8_t ACCESS_BYTE_CODE_CONFORMING = 1 << 2;
static const uint8_t ACCESS_BYTE_DATA_GROWS_DOWN = 1 << 2;
static const uint8_t ACCESS_BYTE_IS_CODE = 3 << 3;
static const uint8_t ACCESS_BYTE_IS_DATA = 2 << 3;
static const uint8_t ACCESS_BYTE_IS_TASK = 1 << 3;
static const uint8_t ACCESS_BYTE_USER = 3 << 5;
static const uint8_t ACCESS_BYTE_PRESENT = 1 << 7;

struct GDT
{
    uint32_t limit;
    uint32_t base;
    uint8_t access_byte;
    uint8_t flags;
};

extern uint32_t gdt;
extern void set_gdt();

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

int GDT_initialize()
{
    uint8_t* gdt_offset = (uint8_t*)gdt;

    struct GDT null;
    null.limit = 0;
    null.base = 0;
    null.access_byte = 0;
    printf("Null access byte: %#.2x\n", null.access_byte);
    null.flags = 0;
    encode_GDT_entry(gdt_offset, null);

    gdt_offset += 8;
    struct GDT kernel_code;
    kernel_code.limit = 0xFFFFF;
    kernel_code.base = 0;
    kernel_code.access_byte = ACCESS_BYTE_CODE_READ | ACCESS_BYTE_IS_CODE | ACCESS_BYTE_PRESENT;
    printf("Kernel code access byte: %#.2x\n", kernel_code.access_byte);
    kernel_code.flags = 0;
    encode_GDT_entry(gdt_offset, kernel_code);

    gdt_offset += 8;
    struct GDT kernel_data;
    kernel_data.limit = 0xFFFFF;
    kernel_data.base = 0;
    kernel_data.access_byte = ACCESS_BYTE_CODE_READ | ACCESS_BYTE_IS_DATA | ACCESS_BYTE_PRESENT;
    printf("Kernel data access byte: %#.2x\n", kernel_data.access_byte);
    kernel_data.flags = 0;
    encode_GDT_entry(gdt_offset, kernel_data);

    gdt_offset += 8;
    struct GDT user_code;
    user_code.limit = 0xFFFFF;
    user_code.base = 0;
    user_code.access_byte = ACCESS_BYTE_CODE_READ | ACCESS_BYTE_IS_CODE | ACCESS_BYTE_USER | ACCESS_BYTE_PRESENT;
    printf("User code access byte: %#.2x\n", user_code.access_byte);
    user_code.flags = 0;
    encode_GDT_entry(gdt_offset, user_code);

    gdt_offset += 8;
    struct GDT user_data;
    user_data.limit = 0xFFFFF;
    user_data.base = 0;
    user_data.access_byte = ACCESS_BYTE_CODE_READ | ACCESS_BYTE_IS_CODE | ACCESS_BYTE_USER | ACCESS_BYTE_PRESENT;
    printf("User code access byte: %#.2x\n", user_data.access_byte);
    user_data.flags = 0;
    encode_GDT_entry(gdt_offset, user_data);

    set_gdt();

    return 0;
}