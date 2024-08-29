#include <stdbool.h>
#include <stdio.h>

#include <init.h>

#include "../gdt/gdt.c"

extern void enter_protected();
extern void load_tss();

void general_initialize()
{
    printf("Initializing GDT...\n");
    GDT_initialize();

    printf("Entering protected mode...\n");
    enter_protected();

    printf("Initialising TSS...\n");
    load_tss();
}