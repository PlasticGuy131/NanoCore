#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <float.h>
#include <kernel.h>
#include <mem_allocation.h>
#include <multiboot.h>
#include <port.h>
#include <serial.h>
#include <terminal.h>

#define VERSION "More Printf Testing"
#ifndef ARCH
#define ARCH "WARNING: Unknown Architecture"
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

void kernel_panic(const char* error_message)
{
    terminal_clear();
    terminal_col_error();
    printf("ERRO/RERR/ORRE/RROR/\n");
    printf("E/RROR/ERRO/RRER/ROR\n");
    printf("ER/RORE/RROR/RERR/OR\n");
    printf("ERR/ORER/RORR/ERRO/R\n");
    terminal_col_default();
    printf("KERNEL PANIC: %s\n", error_message);
}

void* kernel_alloc(size_t size) { return memory_alloc(size); }

void kernel_free(void* ptr) { memory_free(ptr); }

void kernel_main(void)
{
    bool serial_failure = serial_initialize();
    if(_multiboot_magic != MULTIBOOT_MAGIC)
    {
        kernel_panic("Not loaded with multiboot");
        return;
    }

    Multiboot_Info* multiboot_info = (Multiboot_Info*)_multiboot_info_start;
    terminal_initialize(multiboot_info);
    terminal_set_default_colour(COLOUR_WHITE, COLOUR_BLACK);
    terminal_set_warning_colour(COLOUR_LIGHT_BROWN, COLOUR_BLACK);
    terminal_set_error_colour(COLOUR_RED, COLOUR_BLACK);

    if (serial_failure)
    {
        terminal_col_warning();
        printf("Warning: Serial initialization failure");
        terminal_col_default();
    }

    printf("Initializing FPU...\n");
    float_initialize();

    printf("Initializing memory...\n");
    memory_initialize();

    printf("\n");
    kernel_intro_splash();

    printf("testing printf...\n");
    printf("INT 7: %.2d\n", 7);
    printf("OCT 7: %.*o\n", 3, 7);
    printf("HEX 7: %.*2$x\n", 7, 4);
    printf("\n");
    printf("FLOAT 3.5: %.1f\n", 3.5);
    printf("FLOAT 3.5: %f\n", 3.5);
    printf("FLOAT 1.5075: %.1f\n", 1.5075);
    printf("EXP 150.75: %.1e\n", 150.75);
    printf("EXP 150.75: %e\n", 150.75);
    printf("FHEX 3.8: %.4a\n", 3.8);
    printf("FHEX 3.8: %a\n", 3.8);
    printf("GFLOAT 111: %.2g\n", 111.0);
    printf("GFLOAT 111: %g\n", 111.0);
    printf("GFLOAT 0.0101: %.2g\n", 0.0101);
    printf("GFLOAT 0.0101: %#.2g\n", 0.0101);
    printf("GFLOAT 0.0101: %g\n", 0.0101);

    printf("STRING HELLO: %.5s\n", "HELLO:(");

    printf("\nMEMORY USAGE: %i/%i\n", memory_usage(), memory_max());
}