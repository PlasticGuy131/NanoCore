#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <multiboot.h>
#include <port.h>
#include <serial.h>
#include <terminal.h>

#include "kernel.h"

#define VERSION "Multiboot refactor"
#ifndef ARCH
#define ARCH WARNING: Unknown Architecture
#endif

void kernel_putchar(char c) { terminal_putchar(c); }

static void kernel_intro_splash()
{
    terminal_set_colour(COLOUR_RED, COLOUR_BLACK);
    terminal_writestring("NANO/CORE/NANO/CORE/\n");
    terminal_set_colour(COLOUR_GREEN, COLOUR_BLACK);
    terminal_writestring("C/OREN/ANOC/OREN/ANO\n");
    terminal_set_colour(COLOUR_CYAN, COLOUR_BLACK);
    terminal_writestring("NA/NOCO/RENA/NOCO/RE\n");
    terminal_set_colour(COLOUR_BLUE, COLOUR_BLACK);
    terminal_writestring("COR/ENAN/OCOR/ENAN/O\n");
    terminal_set_colour(COLOUR_WHITE, COLOUR_BLACK);
    terminal_writestring("NANO CORE v.");
    terminal_writestring(VERSION);
    terminal_writestring("-");
    terminal_writestring(ARCH);
    terminal_writestring("\nWelcome.\n");
}

void kernel_main(void)
{
    bool serial_failure = serial_initialize();
    if(multiboot_magic != MULTIBOOT_MAGIC)
    {
        terminal_writestring("ERROR: NOT LOADED WITH MULTIBOOT, PANIC!\n");
        return;
    }

    Multiboot_Info* multiboot_info = (Multiboot_Info*)multiboot_info_start;
    terminal_initialize(multiboot_info);

    if (serial_failure)
    {
        terminal_set_colour(COLOUR_LIGHT_BROWN, COLOUR_BLACK);
        terminal_writestring("Warning: Serial initialization failure");
        terminal_set_colour(COLOUR_WHITE, COLOUR_BLACK);
    }

    kernel_intro_splash();

    int v = 27;
    printf("v can be written as: %d, %x, %o", v, v, v);
}