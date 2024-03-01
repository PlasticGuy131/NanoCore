#include <stddef.h>
#include <stdint.h>

#include <multiboot.h>
#include <terminal.h>

#include "psf.c"
#include "screen.c"
#include "vga.c"

extern uint32_t multiboot_info_start;

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

    PSF_Header* font = psf_get_header();

    uint16_t unicode[512];
    char* offset = psf_setup_font(unicode);

    offset += unicode['A'] * font->character_size;
    for (size_t i = 0; i < font->character_size; i++)
    {
        char line = *offset;
        terminal_writebyte(line);
        terminal_writestring("\n");
        offset++;
    }

    Multiboot_Info* multiboot_info = (Multiboot_Info*)multiboot_info_start;

    screen_initialize(multiboot_info);
    for (int x = 1; x < 11; x++)
    {
        for (int y = 1; y < 11; y++)
        {
            screen_putpixel(x, y, screen_rgb(255, 255, 255));
        }
    }
    char data[8] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
    screen_putbitmap_bw(15, 1, data, 1, 8, screen_rgb_name(COLOUR_RED), screen_rgb(COLOUR_BLUE));
}

uint8_t terminal_create_colour(enum Colour fg, enum Colour bg) { return vga_entry_colour(fg, bg); }

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