#include <stddef.h>
#include <stdint.h>

#include <multiboot.h>
#include <terminal.h>

#include "psf.c"
#include "screen.c"
#include "vga.c"

enum Display_Type
{
    DISPLAY_SERIAL_ONLY = 0,
    DISPLAY_RGB = 1,
    DISPLAY_VGA = 2
};

enum Display_Type display_type;
size_t terminal_row;
size_t terminal_column;
size_t terminal_width;
size_t terminal_height;
enum Colour terminal_fg_colour;
enum Colour terminal_bg_colour;

void (*terminal_putcharat)(char, enum Colour, enum Colour, size_t, size_t);
void (*terminal_scroll)();

uint16_t* terminal_buffer;

size_t terminal_font_char_size;
static const size_t terminal_char_width = 9;
uint16_t unicode[512];
char* font_offset;

static inline int terminal_xpixel(size_t x)
{
    return x * terminal_char_width;
}

static inline int terminal_ypixel(size_t y)
{
    return y * terminal_font_char_size;
}

static void terminal_vga_putentryat(char c, enum Colour fg, enum Colour bg, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, vga_entry_colour(fg, bg));
}

static void terminal_rgb_putcharat(char c, enum Colour fg, enum Colour bg, size_t x, size_t y)
{
    char* offset = font_offset;
    offset += unicode[c] * terminal_font_char_size;
    screen_putbitmap_bw(terminal_xpixel(x), terminal_ypixel(y), offset, 1, terminal_font_char_size, screen_rgb_name(fg), screen_rgb_name(bg));
}

static void terminal_vga_scroll()
{
    for (size_t y = 0; y < terminal_height-1; y++)
    {
        for (size_t x = 0; x < terminal_width; x++)
        {
            size_t index = (y + 1) * terminal_width + x;
            uint16_t entry = terminal_buffer[index];
            index -= terminal_width;
            terminal_buffer[index] = entry;
        }
    }

    for (size_t x = 0; x < terminal_width; x++)
    {
        const size_t index = (terminal_height - 1) * terminal_width + x;
        terminal_buffer[index] = vga_entry(' ',  vga_entry_colour(terminal_fg_colour, terminal_bg_colour));
    }
}

static void terminal_rgb_scroll()
{
    for (size_t y = 0; y < (terminal_height-1) * terminal_font_char_size; y++)
    {
        for (size_t x = 0; x < terminal_width * terminal_char_width; x++)
        {
            screen_copypixel(x, y+terminal_font_char_size, x, y);
        }
    }

    for (size_t x = 0; x < terminal_width; x++)
    {
        terminal_putcharat(' ', terminal_fg_colour, terminal_bg_colour, x, terminal_height - 1);
    }
}

static void terminal_vga_initialize(Multiboot_Info* multiboot_info)
{
    uint8_t terminal_colour = vga_entry_colour(COLOUR_LIGHT_GRAY, COLOUR_BLACK);
    terminal_buffer = (uint16_t*)multiboot_info->framebuffer_addr;
    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_colour);
        }
    }

    terminal_height = VGA_HEIGHT;
    terminal_width = VGA_WIDTH;

    terminal_putcharat = &terminal_vga_putentryat;
    terminal_scroll = &terminal_vga_scroll;
}

static void terminal_rgb_initialize(Multiboot_Info* multiboot_info)
{
    PSF_Header* font = psf_get_header();
    terminal_font_char_size = font->character_size;
    font_offset = psf_setup_font(unicode);
    screen_initialize(multiboot_info);

    terminal_height = multiboot_info->framebuffer_height / terminal_font_char_size;
    terminal_width = multiboot_info->framebuffer_width / terminal_char_width;

    terminal_putcharat = &terminal_rgb_putcharat;
    terminal_scroll = &terminal_rgb_scroll;
}

void terminal_initialize(void)
{
    if(multiboot_magic != MULTIBOOT_MAGIC)
    {
        terminal_writestring("ERROR: NOT LOADED WITH MULTIBOOT, PANIC!\n");
        return;
    }

    Multiboot_Info* multiboot_info = (Multiboot_Info*)multiboot_info_start;
    display_type = multiboot_info->framebuffer_type;

    terminal_row = 0;
    terminal_column = 0;

    if (display_type == DISPLAY_RGB)
    {
        terminal_rgb_initialize(multiboot_info);
    }
    else if (display_type == DISPLAY_VGA)
    {
        terminal_vga_initialize(multiboot_info);
    }
    else
    {
        terminal_putcharat = NULL;
        terminal_scroll = NULL;
    }
}

void terminal_putchar(char c)
{
    switch (c)
    {
        case '\n':
            terminal_column = 0;
            if (++terminal_row >= terminal_height) { terminal_scroll(); }
            break;
        default:
            terminal_putcharat(c, terminal_fg_colour, terminal_bg_colour, terminal_column, terminal_row);
            if(++terminal_column >= terminal_width)
            {
                terminal_column = 0;
                if (++terminal_row >= terminal_height) { terminal_scroll(); }
            }
    }
}