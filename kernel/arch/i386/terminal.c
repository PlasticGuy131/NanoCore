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
enum Colour terminal_fg_colour;
enum Colour terminal_bg_colour;

uint16_t* terminal_buffer;

size_t terminal_font_char_size;
static const size_t terminal_char_width = 9;

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
    terminal_char_width = font->character_size;

    uint16_t unicode[512];
    char* offset = psf_setup_font(unicode);

    Multiboot_Info* multiboot_info = (Multiboot_Info*)multiboot_info_start;

    screen_initialize(multiboot_info);

    offset += unicode['A'] * terminal_char_width;
    screen_putbitmap_bw(0, 0, offset, 1, 16, screen_rgb_name(COLOUR_WHITE), screen_rgb_name(COLOUR_BLACK)); 
}

void terminal_setcolour(enum Colour fg, enum Colour bg)
{
    terminal_fg_colour = fg;
    terminal_bg_colour = bg;
}

static void terminal_putentryat(char c, enum Colour fg, enum Colour bg, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, vga_entry_colour(fg, bg));
    //offset += unicode[c] * terminal_char_width;
    //screen_putbitmap_bw(x * terminal_char_width, y * terminal_char_width, offset, 1, 16, fg, bg;
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
            terminal_putentryat(c, terminal_fg_colour, terminal_bg_colour, terminal_column, terminal_row);
            if(++terminal_column >= VGA_WIDTH)
            {
                terminal_column = 0;
                if (++terminal_row >= VGA_HEIGHT) { terminal_scroll(); }
            }
    }
}