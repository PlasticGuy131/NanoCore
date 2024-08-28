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