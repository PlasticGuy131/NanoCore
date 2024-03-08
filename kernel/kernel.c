#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <port.h>
#include <serial.h>
#include <terminal.h>


#define VERSION "LIBC STDIO Testing"
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
    terminal_initialize();

    if (serial_failure)
    {
        terminal_set_colour(COLOUR_LIGHT_BROWN, COLOUR_BLACK);
        terminal_writestring("Warning: Serial initialization failure");
        terminal_set_colour(COLOUR_WHITE, COLOUR_BLACK);
    }

    kernel_intro_splash();

    putchar((int)'A');
    putchar((int)'\n');

    int a = 10;
    int* b = &a;

    printf("NONE\n");
    printf("CHAR: %c\n", 'A');
    printf("STR: %s\n", "STR");
    printf("INT1: %d\n", 7);
    printf("INT2: %i\n", -3);
    printf("UINT1: %u\n", 7);
    printf("UINT2: %u\n", -3);
    printf("OCTAL: %o\n", 88);
    printf("hex: %x\n", 1194684);
    printf("HEX: %X\n", 1194684);
    printf("POINTER: %p\n", b);
    int n;
    printf("NONE: %nFOO\n", &n);
    printf("NONE: %d\n", n);

    char buf[20];
    sprintf(buf, "sprintf: %s", "GO!");
    puts(buf);

    snprintf(buf, "snprintf: %s", 9, "GO!");
    buf[9] = '\0';
    puts(buf);
}