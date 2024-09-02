#include <stdbool.h>
#include <stdio.h>

#include <init.h>
#include <terminal.h>

#include "../gdt/gdt.c"

extern void enter_protected();
extern void load_tss();

void general_initialize()
{
    //printf("Initializing GDT...\n");
    printf(" GDT ");
    terminal_col_warning();
    printf("========>  <========");
    GDT_initialize();
    printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    printf(" ========><======== ");
    terminal_set_colour(COLOUR_GREEN, COLOUR_BLACK);
    printf(" CHECK");
    terminal_col_default();

    printf("Entering protected mode...\n");
    enter_protected();

    printf("Initialising TSS...\n");
    load_tss();
}