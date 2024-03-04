#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <port.h>
#include <serial.h>
#include <terminal.h>


#define VERSION "RGB Scroll testing"
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
        terminal_writestring("Warning: Serial initialization failure");
    }

    kernel_intro_splash();
    serial_write('A');
}