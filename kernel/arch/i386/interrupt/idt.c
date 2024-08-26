#ifdef _MSC_VER
#include <vscode.h>
#endif

#include <stdint.h>

#include <interrupt.h>
#include <kernel.h>

enum Type
{
    TASK = 0x5,
    INTERRUPT_16 = 0x6,
    TRAP_16 = 0x7,
    INTERRUPT_32 = 0xE,
    TRAP_32 = 0xF
};

struct IDT
{
    uint32_t offset;
    uint16_t selector;
    uint8_t type;
    uint8_t dpl;
};

struct __attribute__((packed)) IDTR
{
    uint16_t limit;
    uint32_t base;
};

extern void load_idt(uint32_t idtr);

__attribute__((aligned(0x10))) static uint8_t idt[256 * 8];
static struct IDTR idtr;

static void encode_IDT_entry(uint8_t* target, struct IDT source)
{
    target[0] = source.offset & 0xFF;
    target[1] = (source.offset >> 8) & 0xFF;
    target[6] = (source.offset >> 16) & 0xFF;
    target[7] = (source.offset >> 24) & 0xFF;

    target[2] = source.selector & 0xFF;
    target[3] = (source.selector >> 8) & 0xFF;

    target[5] = source.type | (source.dpl << 5) | 1 << 8;

    target[4] = 0;
}

static void load_idtr()
{
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = 32;

    load_idt((uintptr_t)&idtr);
}

int interrupt_initialize()
{
    load_idtr();
    return 0;
}