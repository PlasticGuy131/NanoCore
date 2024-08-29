#ifdef _MSC_VER
#include <vscode.h>
#endif

#include <stdint.h>

#include <kernel.h>
#include <interrupt.h>
#include <pic.h>

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

extern void load_idt();
extern void enable_interrupts();

__attribute__((aligned(0x10))) static uint8_t idt[256 * 8];
extern struct IDTR idtr;

static void encode_IDT_entry(uint8_t* target, struct IDT source)
{
    target[0] = source.offset & 0xFF;
    target[1] = (source.offset >> 8) & 0xFF;
    target[6] = (source.offset >> 16) & 0xFF;
    target[7] = (source.offset >> 24) & 0xFF;

    target[2] = source.selector & 0xFF;
    target[3] = (source.selector >> 8) & 0xFF;

    target[5] = source.type | (source.dpl << 5) | 1 << 7;

    target[4] = 0;
}

static void load_idtr()
{
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = 34 * 8;

    load_idt();
}

int interrupt_initialize()
{
    pic_disable();
    struct IDT idt_entry;
    idt_entry.selector = 0x08;
    idt_entry.dpl = 0;
    idt_entry.type = INTERRUPT_32;

    idt_entry.offset = (uintptr_t)&interrupt_div_zero_wpr;
    encode_IDT_entry(idt + 8 * 0, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_debug_wpr;
    encode_IDT_entry(idt + 8 * 1, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_nmi_wpr;
    encode_IDT_entry(idt + 8 * 2, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_breakpoint_wpr;
    encode_IDT_entry(idt + 8 * 3, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_overflow_wpr;
    encode_IDT_entry(idt + 8 * 4, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_bound_wpr;
    encode_IDT_entry(idt + 8 * 5, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_invalid_opcode_wpr;
    encode_IDT_entry(idt + 8 * 6, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_device_wpr;
    encode_IDT_entry(idt + 8 * 7, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_double_wpr;
    encode_IDT_entry(idt + 8 * 8, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_tss_wpr;
    encode_IDT_entry(idt + 8 * 10, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_segment_wpr;
    encode_IDT_entry(idt + 8 * 11, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_stack_segment_wpr;
    encode_IDT_entry(idt + 8 * 12, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_general_protection_wpr;
    encode_IDT_entry(idt + 8 * 13, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_page_wpr;
    encode_IDT_entry(idt + 8 * 14, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_floating_wpr;
    encode_IDT_entry(idt + 8 * 16, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_alignment_wpr;
    encode_IDT_entry(idt + 8 * 17, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_machine_wpr;
    encode_IDT_entry(idt + 8 * 18, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_SIMD_wpr;
    encode_IDT_entry(idt + 8 * 19, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_virtualization_wpr;
    encode_IDT_entry(idt + 8 * 20, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_control_wpr;
    encode_IDT_entry(idt + 8 * 21, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_hypervisor_wpr;
    encode_IDT_entry(idt + 8 * 28, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_VMM_wpr;
    encode_IDT_entry(idt + 8 * 29, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_security_wpr;
    encode_IDT_entry(idt + 8 * 30, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_clock_wpr;
    encode_IDT_entry(idt + 8 * 32, idt_entry);

    idt_entry.offset = (uintptr_t)&interrupt_keyboard_wpr;
    encode_IDT_entry(idt + 8 * 33, idt_entry);

    load_idtr();
    enable_interrupts();

    pic_initialize();

    return 0;
}