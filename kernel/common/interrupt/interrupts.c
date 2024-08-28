#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <interrupt.h>
#include <kernel.h>

enum DESCRIPTOR_TABLE
{
    DESCRIPTOR_GDT = 0,
    DESCRIPTOR_IDT = 1,
    DESCRIPTOR_LDT = 2
};

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

void interrupt_panic() { kernel_panic("Unknown Exception Occurred!"); }
void interrupt_arg_test(uint32_t arg) { printf("%d\n", arg); }

void interrupt_div_zero() { kernel_panic("Exception Occurred: Division Error"); }
void interrupt_debug() { kernel_panic("Exception Occurred: Debug (not supported)"); }
void interrupt_nmi() { kernel_panic("Exception Occurred: Hardware Error"); }
void interrupt_breakpoint() { kernel_panic("Exception Occurred: Breakpoint (not supported)"); }
void interrupt_overflow() { kernel_panic("Exception Occurred: Overflow Error"); }
void interrupt_bound() { kernel_panic("Exception Occurred: Bound range exceeded"); }
void interrupt_invalid_opcode() { kernel_panic("Exception Occurred: Invalid Opcode"); }
void interrupt_device() { kernel_panic("Exception Occurred: Device not available"); }
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

void interrupt_page(uint32_t error) { kernel_panic("Exception Occurred: Page Fault"); } //TO-DO: parse error code
void interrupt_floating() { kernel_panic("Exception Occurred: x86 Floating Point Exception"); }
void interrupt_alignment() { kernel_panic("Exception Occurred: Alignment Check Failure"); }
void interrupt_machine() { kernel_panic("Exception Occurred: Machine Check Failure"); }
void interrupt_SIMD() { kernel_panic("Exception Occurred: SIMD Floating Point Exception"); }
void interrupt_virtualization() { kernel_panic("Exception Occurred: Virtualization Exception"); }
void interrupt_control() { kernel_panic("Exception Occurred: Control protection Exception"); }
void interrupt_hypervisor() { kernel_panic("Exception Occurred: Hypervisior Injection Exception"); }
void interrupt_VMM() { kernel_panic("Exception Occurred: VMM Communication Exception"); }
void interrupt_security() { kernel_panic("Exception Occurred: Security Exception"); }
