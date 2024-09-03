#include <stdbool.h>
#include <stdio.h>

#include <clock.h>
#include <init.h>
#include <terminal.h>

#include "../gdt/gdt.c"

extern void enter_protected();
extern void load_tss();

void general_initialize()
{
    //printf("Initializing GDT...\n");
    GDT_initialize();
    init_print("GDT", true, false);

    ///printf("Entering protected mode...\n");
    terminal_col_warning();
    printf("=======BORDERLINE==PROTECTED=======\n");
    terminal_col_default();
    enter_protected();

    //printf("Initialising TSS...\n");
    load_tss();
    init_print("TSS", true, false);
}

void init_print(const char* init, bool success, bool sleep)
{
    printf(" %-6s ", init);
    terminal_col_warning();
    if (sleep)
    {
        printf("========>  <========");
        clock_sleep(200);
        printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    }
    printf(" ========><======== ");
    if (sleep) { clock_sleep(200); }
    if (success)
    {
        terminal_set_colour(COLOUR_GREEN, COLOUR_BLACK);
        printf(" CHECK\n");
    }
    else
    {
        terminal_col_error();
        printf(" FAILURE\n");
    }
    terminal_col_default();
}