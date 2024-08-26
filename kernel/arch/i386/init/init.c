#include <stdbool.h>
#include <stdio.h>

#include <init.h>

#include "../gdt/gdt.c"

extern void enter_protected();
extern void load_tss();

int general_initialize()
{
    bool result = 0;
    printf("Initializing GDT...");
    result |= GDT_initialize();

    printf("Entering protected mode...\n");
    enter_protected();

    printf("Initialising TSS...\n");
    load_tss();

    return result;
}