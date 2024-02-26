extern char _binary_font_psf_start[];

#include <stddef.h>
#include <stdint.h>

#include <terminal.h>

#include "psf.c"
#include "vga.c"

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_colour;
uint16_t* terminal_buffer;


void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_colour = vga_entry_colour(COLOUR_LIGHT_GRAY, COLOUR_BLACK);
    terminal_buffer = (uint16_t *)0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_colour);
        }
    }

    uint16_t unicode[512] = psf_generate_table();

    PSF1_Header* font = (PSF1_Header*)&_binary_font_psf_start;
    terminal_writestring("A: ");
    terminal_writeint('A');
    terminal_writestring(" -> ");
    terminal_writeint(unicode['A']);
    terminal_writestring("B: ");
    terminal_writeint('B');
    terminal_writestring(" -> ");
    terminal_writeint(unicode['B']);
}

uint8_t terminal_create_colour(enum colour fg, enum colour bg) { return vga_entry_colour(fg, bg); }

void terminal_setcolour(uint8_t colour)
{
    terminal_colour = colour;
}

static void terminal_putentryat(char c, uint8_t colour, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, colour);
}

static void terminal_scroll()
{
    for (size_t y = 0; y < VGA_HEIGHT-1; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            size_t index = (y + 1) * VGA_WIDTH + x;
            uint16_t entry = terminal_buffer[index];
            index -= VGA_WIDTH;
            terminal_buffer[index] = entry;
        }
    }

    for (size_t x = 0; x < VGA_WIDTH; x++)
    {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        terminal_buffer[index] = vga_entry(' ', terminal_colour);
    }

    terminal_row = VGA_HEIGHT - 1;
}

void terminal_putchar(char c)
{
    switch (c)
    {
        case '\n':
            terminal_column = 0;
            if (++terminal_row >= VGA_HEIGHT) { terminal_scroll(); }
            break;
        default:
            terminal_putentryat(c, terminal_colour, terminal_column, terminal_row);
            if(++terminal_column >= VGA_WIDTH)
            {
                terminal_column = 0;
                if (++terminal_row >= VGA_HEIGHT) { terminal_scroll(); }
            }
    }
}