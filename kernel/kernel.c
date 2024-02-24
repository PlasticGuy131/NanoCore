#include <stddef.h>
#include <stdint.h>

#include <terminal.h>

void kernel_intro_splash()
{
    terminal_setcolour(vga_entry_colour(VGA_COLOUR_RED, VGA_COLOUR_BLACK));
    terminal_writestring("NANO/CORE/NANO/CORE/\n");
    terminal_setcolour(vga_entry_colour(VGA_COLOUR_GREEN, VGA_COLOUR_BLACK));
    terminal_writestring("C/OREN/ANOC/OREN/ANO\n");
    terminal_setcolour(vga_entry_colour(VGA_COLOUR_CYAN, VGA_COLOUR_BLACK));
    terminal_writestring("NA/NOCO/RENA/NOCO/RE\n");
    terminal_setcolour(vga_entry_colour(VGA_COLOUR_BLUE, VGA_COLOUR_BLACK));
    terminal_writestring("COR/ENAN/OCOR/ENAN/O\n");
    terminal_setcolour(vga_entry_colour(VGA_COLOUR_WHITE, VGA_COLOUR_BLACK));
    terminal_writestring("NANO CORE v.");
    terminal_writestring(VERSION);
    terminal_writestring("\nWelcome.\n");
}

void kernel_main(void)
{
    terminal_initialize();

    terminal_intro_splash();
    terminal_writestring("Testing scrolling:");
    for (size_t i = 0; i < 100; i++)
    {
        char str[4] = " N\n";
        int out = i;
        if (i >= 10)
        {
            int tens = i / 10;
            str[0] = tens + '0';
            out -= tens * 10;
        }
        str[1] = out + '0';
        terminal_writestring(str);
    }
}