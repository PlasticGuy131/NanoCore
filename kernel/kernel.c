#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <port.h>
#include <serial.h>
#include <terminal.h>


#define VERSION "LIBC String Testing"
#ifndef ARCH
#define ARCH WARNING: Unknown Architecture
#endif

void kernel_intro_splash()
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
    char test1[13];
    memcpy(test1, "MEMCPY TEST\n", 13);
    terminal_writestring(test1);
    char test2[13];
    memcpy(test2, test1, 13);
    terminal_writestring("MEMCMP TEST (0 / 1)\n");
    terminal_writeint(memcmp(test1, test2, 13));
    memcpy(test2, "aaaaaaaaaaaa", 13);
    terminal_writestring(" ");
    terminal_writeint(memcmp(test1, test2, 13));
    uint16_t* val;
    memset(val, 1, 2);
    terminal_writestring("\nMEMSET TEST\n");
    terminal_writeint(*val);
}