#ifdef _MSC_VER
#include <vscode.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clock.h>
#include <float_init.h>
#include <init.h>
#include <interrupt.h>
#include <kernel.h>
#include <keyboard.h>
#include <mem_allocation.h>
#include <multiboot.h>
#include <port.h>
#include <serial.h>
#include <terminal.h>

#define VERSION "Typing Interface"
#ifndef ARCH
#define ARCH "WARNING: Unknown Architecture"
#endif

void kernel_putchar(unsigned char c) { terminal_putchar(c); }

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

static void kernel_type(Keypress keypress)
{
    if (keypress.flags & KEY_FLAG_PRESSED)
    {
        unsigned char c = keyboard_keypress_char(keypress);
        if (c)
        {
            printf("%c", c);
        }
        else if (keypress.code == KEYCODE_CURSOR_LEFT) { terminal_cursor_left(); }
    }
}

static void kernel_initialize()
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
        printf("Warning: Serial initialization failure\n");
        terminal_col_default();
    }

    memory_initialize();
    init_print("MEMORY", true, false);
    
    general_initialize();

    interrupt_initialize();
    init_print("IDT", true, false);

    clock_initialize();
    init_print("CLOCK", true, true);

    bool float_failed = float_initialize();
    init_print("FPU", !float_failed, true);
    if (float_failed)
    {
        terminal_col_error();
        printf("ERROR: FLOAT INITIALIZATION FAILED\n");
        terminal_col_default();
    }
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
    while (1);
}

void* kernel_alloc(size_t size) { return memory_alloc(size); }

void kernel_free(void* ptr) { memory_free(ptr); }

void kernel_main(void)
{
    kernel_initialize();

    printf("\n");
    kernel_intro_splash();

    keyboard_register_callback(kernel_type);
    terminal_cursor_enable();
    /*while (1)
    {
        terminal_cursor_blink();
        clock_sleep(500);
    }*/
}