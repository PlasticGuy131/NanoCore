#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <float.h>
#include <kernel.h>
#include <multiboot.h>
#include <port.h>
#include <serial.h>
#include <terminal.h>

#define VERSION "printf tests"
#ifndef ARCH
#define ARCH WARNING: Unknown Architecture
#endif

void kernel_putchar(char c) { terminal_putchar(c); }

static void kernel_intro_splash()
{
    terminal_set_colour(COLOUR_RED, COLOUR_BLACK);
    printf("NANO/CORE/NANO/CORE/\n");
    terminal_set_colour(COLOUR_GREEN, COLOUR_BLACK);
    printf("C/OREN/ANOC/OREN/ANO\n");
    terminal_set_colour(COLOUR_CYAN, COLOUR_BLACK);
    printf("NA/NOCO/RENA/NOCO/RE\n");
    terminal_set_colour(COLOUR_BLUE, COLOUR_BLACK);
    printf("COR/ENAN/OCOR/ENAN/O\n");
    terminal_set_colour(COLOUR_WHITE, COLOUR_BLACK);
    printf("NANO CORE v.");
    printf(VERSION);
    printf("-");
    printf(ARCH);
    printf("\nWelcome.\n");
}

void kernel_main(void)
{
    bool serial_failure = serial_initialize();
    if(_multiboot_magic != MULTIBOOT_MAGIC)
    {
        printf("ERROR: NOT LOADED WITH MULTIBOOT, PANIC!\n");
        return;
    }

    Multiboot_Info* multiboot_info = (Multiboot_Info*)_multiboot_info_start;
    terminal_initialize(multiboot_info);

    if (serial_failure)
    {
        terminal_set_colour(COLOUR_LIGHT_BROWN, COLOUR_BLACK);
        printf("Warning: Serial initialization failure");
        terminal_set_colour(COLOUR_WHITE, COLOUR_BLACK);
    }

    printf("Initializing FPU...\n");
    if (float_initialize())
    {
        terminal_set_colour(COLOUR_RED, COLOUR_BLACK);
        printf("ERROR: FLOAT INITIALIZATION FAILED\n");
        terminal_set_colour(COLOUR_WHITE, COLOUR_BLACK);
    }

    printf("\n");
    kernel_intro_splash();

    printf("testing printf...\n");
    double f = 5.0000001;
    printf("FLOAT: %g\n", f);
    printf("FLOAT: %f\n", f);
    f = 5.1;
    printf("FLOAT: %G\n", f);
    printf("FLOAT: %f\n", f);
    f = 100000000;
    printf("FLOAT: %g\n", f);
    printf("FLOAT: %f\n", f);
}