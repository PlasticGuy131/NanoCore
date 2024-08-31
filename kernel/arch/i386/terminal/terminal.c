#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <multiboot.h>
#include <serial.h>
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

static const size_t TERMINAL_CHAR_WIDTH = 9;

static const uint8_t CURSOR_FULL_VALUE = 0xC0;

static enum Display_Type display_type;
static size_t terminal_row;
static size_t terminal_column;
static size_t terminal_width;
static size_t terminal_height;

static enum Colour terminal_fg_colour;
static enum Colour terminal_bg_colour;

static enum Colour terminal_default_fg_colour;
static enum Colour terminal_default_bg_colour;

static enum Colour terminal_warning_fg_colour;
static enum Colour terminal_warning_bg_colour;

static enum Colour terminal_error_fg_colour;
static enum Colour terminal_error_bg_colour;

static void (*terminal_putcharat)(unsigned char, enum Colour, enum Colour, size_t, size_t);
static void (*terminal_scroll)();

static uint16_t* terminal_buffer;

static size_t terminal_font_char_size;
static uint16_t unicode[512];
static unsigned char* font_offset;

static unsigned char text_buffer[100000];
static unsigned text_offset = 0;

static bool cursor_on = false;
static uint8_t* cursor_full;

static inline int terminal_xpixel(size_t x)
{
    return x * TERMINAL_CHAR_WIDTH;
}

static inline int terminal_ypixel(size_t y)
{
    return y * terminal_font_char_size;
}

static void terminal_vga_putentryat(unsigned char c, enum Colour fg, enum Colour bg, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, vga_entry_colour(fg, bg));
}

static void terminal_rgb_putcharat(unsigned char c, enum Colour fg, enum Colour bg, size_t x, size_t y)
{
    unsigned char* offset = font_offset;
    offset += unicode[(size_t)c] * terminal_font_char_size;
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

    terminal_row = terminal_height - 1;
}

static void terminal_rgb_scroll()
{
    for (size_t y = 0; y < (terminal_height-1) * terminal_font_char_size; y++)
    {
        for (size_t x = 0; x < terminal_width * TERMINAL_CHAR_WIDTH; x++)
        {
            screen_copypixel(x, y+terminal_font_char_size, x, y);
        }
    }

    for (size_t x = 0; x < terminal_width; x++)
    {
        terminal_putcharat(' ', terminal_fg_colour, terminal_bg_colour, x, terminal_height - 1);
    }

    terminal_row = terminal_height - 1;
}

static void terminal_vga_initialize(Multiboot_Info* multiboot_info)
{
    uint8_t terminal_colour = vga_entry_colour(COLOUR_LIGHT_GRAY, COLOUR_BLACK);
    terminal_buffer = (uint16_t*)(uintptr_t)multiboot_info->framebuffer_addr;
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
    terminal_width = multiboot_info->framebuffer_width / TERMINAL_CHAR_WIDTH;

    terminal_putcharat = &terminal_rgb_putcharat;
    terminal_scroll = &terminal_rgb_scroll;

    cursor_full = malloc(terminal_font_char_size);
    for (size_t i = 0; i < terminal_font_char_size; i++) { cursor_full = CURSOR_FULL_VALUE; }
}

void terminal_initialize(Multiboot_Info* multiboot_info)
{
    if (!(multiboot_info->flags & MULTIBOOT_FLAG_FRAMEBUFFER))
    {
        printf("WARNING: No framebuffer available: serial output only\n");
        terminal_putcharat = NULL;
        terminal_scroll = NULL;
    }

    display_type = multiboot_info->framebuffer_type;

    terminal_row = 0;
    terminal_column = 0;

    terminal_set_colour(COLOUR_WHITE, COLOUR_BLACK);
    if (display_type == DISPLAY_RGB)
    {
        terminal_rgb_initialize(multiboot_info);
        printf("INFO: Terminal initialized with RGB framebuffer\n");
        
    }
    else if (display_type == DISPLAY_VGA)
    {
        terminal_vga_initialize(multiboot_info);
        printf("INFO: Terminal initialized with VGA text\n");
    }
    else
    {
        printf("WARNING: Terminal display type not supported: serial output only\n");
        terminal_putcharat = NULL;
        terminal_scroll = NULL;
    }
}

void terminal_col_default() { terminal_set_colour(terminal_default_fg_colour, terminal_default_bg_colour); }
void terminal_col_warning() { terminal_set_colour(terminal_warning_fg_colour, terminal_warning_bg_colour); }
void terminal_col_error() { terminal_set_colour(terminal_error_fg_colour, terminal_error_bg_colour); }

void terminal_set_colour(enum Colour fg, enum Colour bg)
{
    terminal_fg_colour = fg;
    terminal_bg_colour = bg;
}

void terminal_set_default_colour(enum Colour fg, enum Colour bg)
{
    terminal_default_fg_colour = fg;
    terminal_default_bg_colour = bg;
}

void terminal_set_warning_colour(enum Colour fg, enum Colour bg)
{
    terminal_warning_fg_colour = fg;
    terminal_warning_bg_colour = bg;
}

void terminal_set_error_colour(enum Colour fg, enum Colour bg)
{
    terminal_error_fg_colour = fg;
    terminal_error_bg_colour = bg;
}

void terminal_clear()
{
    if (display_type == DISPLAY_VGA)
    {
        for (size_t i = 0; i < terminal_height; i++) { terminal_putchar('\n'); }
        
    }
    else if (display_type == DISPLAY_RGB)
    {
        screen_fill(0, 0, terminal_width * 8, terminal_height * terminal_font_char_size, screen_rgb_name(terminal_bg_colour));
    }
    terminal_row = 0;
    terminal_column = 0;
}

void terminal_putchar(unsigned char c)
{
    switch (c)
    {
        case '\n':
            serial_write('\r');
            serial_write('\n');
            terminal_column = 0;
            if (++terminal_row >= terminal_height) { terminal_scroll(); }
            break;
        case '\t':
            serial_write('\t');
            terminal_column += 4;
            if(terminal_column >= terminal_width)
            {
                terminal_column = 0;
                if (++terminal_row >= terminal_height) { terminal_scroll(); }
            }
            break;
        case '\b':
            serial_write('\b');
            if (terminal_column) { terminal_column--; }
            else
            {
                terminal_column = terminal_width;
                if (terminal_row) { terminal_row--; }
            }
            terminal_putcharat(' ', terminal_fg_colour, terminal_bg_colour, terminal_column, terminal_row);
            break;
        case 127:
            serial_write(127);
            terminal_putcharat(' ', terminal_fg_colour, terminal_bg_colour, terminal_column, terminal_row);
            break;
        default:
            serial_write(c);
            terminal_putcharat(c, terminal_fg_colour, terminal_bg_colour, terminal_column, terminal_row);
            if(++terminal_column >= terminal_width)
            {
                terminal_column = 0;
                if (++terminal_row >= terminal_height) { terminal_scroll(); }
            }
    }
}

void terminal_cursor_blink()
{
    if (display_type == DISPLAY_RGB)
    {
        if (cursor_on) { terminal_putcharat(' ', terminal_fg_colour, terminal_bg_colour, terminal_column, terminal_row); }
        else
        {
            screen_putbitmap_bw(terminal_xpixel(terminal_column), terminal_ypixel(terminal_row), cursor_full, 1,
                terminal_font_char_size, screen_rgb_name(terminal_fg_colour), screen_rgb_name(terminal_bg_colour));
        }
    }
    cursor_on = !cursor_on;
}