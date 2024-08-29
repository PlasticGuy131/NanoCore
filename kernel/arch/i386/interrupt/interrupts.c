#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <clock.h>
#include <interrupt.h>
#include <kernel.h>

enum DESCRIPTOR_TABLE
{
    DESCRIPTOR_GDT = 0,
    DESCRIPTOR_IDT = 1,
    DESCRIPTOR_LDT = 2
};

void (*interrupt_callback)();

static bool try_return()
{
    if (interrupt_callback)
    {
        interrupt_callback();
        return true;
    }
    else { return false; }
}

static bool selector_is_external(uint32_t selector) { return selector & 1; }

static enum DESCRIPTOR_TABLE selector_table(uint32_t selector)
{
    selector >>= 1;
    selector &= 0x3;
    if (selector == DESCRIPTOR_GDT) { return DESCRIPTOR_GDT; }
    if (selector == DESCRIPTOR_LDT) { return DESCRIPTOR_LDT; }
    return DESCRIPTOR_IDT;
}

static int selector_index(uint32_t selector) { return (selector >> 3) & 0x1FFF; }

static void selector_info(char* buf, uint32_t selector)
{
    char table[4];

    switch(selector_table(selector))
    {
    case (DESCRIPTOR_GDT):
        strcpy(table, "GDT");
        break;
    case (DESCRIPTOR_IDT):
        strcpy(table, "IDT");
        break;
    case (DESCRIPTOR_LDT):
        strcpy(table, "LDT");
        break;
    }

    sprintf(buf, "Extrn: %.0d\nTable: %s\n Index: %#.5x", selector_is_external(selector), table, selector_index(selector));
}

void interrupt_register_callback(void (*callback)()) { interrupt_callback = callback; }
void interrupt_end_callback() { interrupt_callback = 0; }

void interrupt_div_zero() { if (!try_return()) { kernel_panic("Exception Occurred: Division Error"); } }
void interrupt_debug() { kernel_panic("Exception Occurred: Debug (not supported)"); }
void interrupt_nmi() { kernel_panic("Exception Occurred: Hardware Error"); }
void interrupt_breakpoint() { kernel_panic("Exception Occurred: Breakpoint (not supported)"); }
void interrupt_overflow() { if (!try_return()) { kernel_panic("Exception Occurred: Overflow Error"); } }
void interrupt_bound() { if (!try_return()) { kernel_panic("Exception Occurred: Bound range exceeded"); } }
void interrupt_invalid_opcode() { if (!try_return()) { kernel_panic("Exception Occurred: Invalid Opcode"); } }
void interrupt_device() { if (!try_return()) { kernel_panic("Exception Occurred: Device not available"); } }
void interrupt_double() { kernel_panic("DOUBLE FAULT OCCURRED: UNRECOVERABLE"); }

void interrupt_tss(uint32_t selector)
{
    char info[35];
    selector_info(info, selector);
    char buf[66];
    sprintf(buf, "Exception Occurred: TSS Error\n%s", info);
    kernel_panic(buf);
}

void interrupt_segment(uint32_t selector)
{
    char info[35];
    selector_info(info, selector);
    char buf[76];
    sprintf(buf, "Exception Occurred: Segment Not Present\n%s", info);
    kernel_panic(buf);
}

void interrupt_stack_segment(uint32_t selector)
{
    char info[35];
    selector_info(info, selector);
    char buf[98];
    if (selector) { sprintf(buf, "Exception Occurred: Stack Segment Not Present or Out of Limit\n%s", info); }
    else { sprintf(buf, "Exception Occurred: Stack Segment Error"); }
    kernel_panic(buf);
}

void interrupt_general_protection(uint32_t selector)
{
    char info[35];
    selector_info(info, selector);
    char buf[96];
    if (selector) { sprintf(buf, "Exception Occurred: General Protection Fault Due to Segemnt\n%s", info); }
    else { sprintf(buf, "Exception Occurred: Misc General Protection Fault"); }
    kernel_panic(buf);
}

void interrupt_page(uint32_t error)
{
    char buf[40];
    sprintf(buf, "Exception Occurred: Page Fault (%#.4x)", error);
    kernel_panic(buf);
}

void interrupt_floating() { kernel_panic("Exception Occurred: x86 Floating Point Exception"); }
void interrupt_alignment() { kernel_panic("Exception Occurred: Alignment Check Failure"); }
void interrupt_machine() { kernel_panic("Exception Occurred: Machine Check Failure"); }
void interrupt_SIMD() { kernel_panic("Exception Occurred: SIMD Floating Point Exception"); }
void interrupt_virtualization() { kernel_panic("Exception Occurred: Virtualization Exception"); }
void interrupt_control() { kernel_panic("Exception Occurred: Control protection Exception"); }
void interrupt_hypervisor() { kernel_panic("Exception Occurred: Hypervisior Injection Exception"); }
void interrupt_VMM() { kernel_panic("Exception Occurred: VMM Communication Exception"); }
void interrupt_security() { kernel_panic("Exception Occurred: Security Exception"); }

void interrupt_clock()
{
    clock_increment();
    pic_send_eoi(0);
}

void interrupt_keyboard()
{
    printf("Keyboard Interrupt\n");
    pic_send_eoi(1);
}
