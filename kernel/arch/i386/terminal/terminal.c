#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

static const uint8_t CURSOR_FULL_VALUE = 0x80;

static enum Display_Type display_type;
static size_t terminal_row = 0;
static size_t terminal_column = 1;
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
static enum Colour foreground_colours[100000];
static enum Colour background_colours[100000];

static unsigned text_offset = 0;
static unsigned drawing_from = 0;

static bool cursor_enabled = false;
static bool cursor_active = false;
static uint8_t* cursor_full = 0;

static unsigned cursor_x = 0;
static unsigned cursor_y = 0;

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

static int terminal_draw_char(char c, bool draw, enum Colour fg, enum Colour bg)
{
    switch (c)
    {
    case '\n':
        terminal_column = 0;
        if (++terminal_row >= terminal_height)
        {
            terminal_row = terminal_height - 1;
            return 1;
        }
        break;
    case '\t':
        for (int i = 0; i < 4;)
        {
            if (draw) { terminal_putcharat(' ', fg, bg, terminal_column, terminal_row); }
            if (++terminal_column >= terminal_width)
            {
                terminal_column = 0;
                if (++terminal_row >= terminal_height)
                {
                    terminal_row = terminal_height - 1;
                    return 1;
                }
            }
        }
            break;
    default:
        if (draw) { terminal_putcharat(c, fg, bg, terminal_column, terminal_row); }
        if (++terminal_column >= terminal_width)
        {
            terminal_column = 0;
            if (++terminal_row >= terminal_height)
            {
                terminal_row = terminal_height - 1;
                return 1;
            }
        }
    }
    return 0;
}

static void terminal_rebase(unsigned stop)
{
    terminal_row = 0;
    terminal_column = 0;
    for (unsigned i = drawing_from; i < stop; i++) { terminal_draw_char(text_buffer[i], false, COLOUR_BLACK, COLOUR_BLACK); }
}

static void terminal_redraw_from(unsigned from)
{
    terminal_rebase(from);
    unsigned i = from;
    while (text_buffer[i])
    {
        if (terminal_draw_char(text_buffer[i], true, foreground_colours[i], background_colours[i])) { break; }
        i++;
    }
}

static void terminal_rgb_scroll()
{
    for (unsigned i = 0; i < terminal_width; i++)
    {
        drawing_from++;
        if (text_buffer[drawing_from] == '\n')
        {
            drawing_from++;
            break;
        }
    }

    for (unsigned y = 0; y < terminal_height; y++)
    {
        for (unsigned x = 0; x < terminal_width; x++) { terminal_putcharat(' ', terminal_fg_colour, terminal_bg_colour, x, y); }
    }

    terminal_redraw_from(drawing_from);

    cursor_y = terminal_height - 1;
}

static void terminal_rgb_scroll_up()
{
    if (drawing_from != 0)
    {
        if (text_buffer[drawing_from - 1] == '\n') { drawing_from--; }
        for (unsigned i = 0; i < terminal_width; i++)
        {
            if (drawing_from == 0) { break; }
            drawing_from--;
            if (text_buffer[drawing_from] == '\n')
            {
                drawing_from++;
                break;
            }
        }

        for (unsigned y = 0; y < terminal_height; y++)
        {
            for (unsigned x = 0; x < terminal_width; x++) { terminal_putcharat(' ', terminal_fg_colour, terminal_bg_colour, x, y); }
        }

        terminal_redraw_from(drawing_from);
    }

    cursor_y = 0;
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

static void terminal_rgb_draw(unsigned char* start)
{
    for (size_t i = 0; i < terminal_width * terminal_height; i++)
    {
        //if (!start[i]) { break; }
        switch (start[i])
        {
        case '\n':
            break;

        default:
            terminal_putcharat(start[i], terminal_fg_colour, terminal_bg_colour, i % terminal_width, i / terminal_width);
            break;
        }
    }
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

    terminal_rgb_draw(text_buffer);
}

static void write_to_buffers(char c, unsigned offset)
{
    text_buffer[offset] = c;
    foreground_colours[offset] = terminal_fg_colour;
    background_colours[offset] = terminal_bg_colour;
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
    serial_write(c);

    switch (c)
    {
    case '\n':
        unsigned move = terminal_width - (text_offset % terminal_width);
        memset(text_buffer + text_offset, '\n', move - 1);
        text_offset += move;
        break;
    case '\t':
        memset(text_buffer + text_offset, ' ', 4);
        text_offset += 5;
        break;
    default:
        text_buffer[text_offset] = c;
        text_offset++;
        break;
    }

    terminal_rgb_draw(text_buffer);

    /*return;

    if (cursor_enabled)
    {
        screen_fill(terminal_xpixel(cursor_x) - 1, terminal_ypixel(cursor_y), 1, terminal_font_char_size, colours[terminal_bg_colour]);
        cursor_active = false;
    }

    switch (c)
    {
    case '\b':
        if (text_offset == 0) { break; }
        terminal_rebase(text_offset);
        terminal_putcharat(' ', terminal_fg_colour, terminal_bg_colour, terminal_column, terminal_row);
        text_offset--;
        write_to_buffers(' ', text_offset);
        if (cursor_x == 0 && cursor_y != 0 && display_type == DISPLAY_RGB) { terminal_rgb_scroll_up(); }
        break;
    case '\n':
        terminal_rebase(text_offset);
        terminal_putcharat(' ', terminal_fg_colour, terminal_bg_colour, terminal_column, terminal_row);
        write_to_buffers(c, text_offset);
        if (++cursor_y >= terminal_height) { terminal_scroll(); }
        text_offset++;
        break;
    case '\t':
        write_to_buffers(c, text_offset);
        cursor_x += 4;
        if (cursor_x >= terminal_width)
        {
            cursor_x = 0;
            if (++cursor_y >= terminal_height) { terminal_scroll(); }
        }
        text_offset++;
        break;
    default:
        write_to_buffers(c, text_offset);
        if (++cursor_x >= terminal_width)
        {
            if (++cursor_y >= terminal_height) { terminal_scroll(); }
        }
        text_offset++;
        break;
    }
    if (text_offset) { terminal_redraw_from(text_offset - 1); }
    terminal_rebase(text_offset);
    cursor_x = terminal_column;
    cursor_y = terminal_row;

    if (cursor_enabled)
    {
        screen_fill(terminal_xpixel(cursor_x) - 1, terminal_ypixel(cursor_y), 1, terminal_font_char_size, colours[terminal_fg_colour]);
        cursor_active = true;
    }*/
}

void terminal_cursor_blink()
{
    if (display_type == DISPLAY_RGB && cursor_enabled)
    {
        terminal_rebase(text_offset);
        cursor_x = terminal_column;
        cursor_y = terminal_row;

        screen_fill(terminal_xpixel(cursor_x) - 1, terminal_ypixel(cursor_y), 1, terminal_font_char_size, colours[cursor_active ? terminal_bg_colour : terminal_fg_colour]);
        cursor_active = !cursor_active;
    }
}

void terminal_cursor_enable()
{
    if (cursor_full == 0)
    {
        cursor_full = malloc(terminal_font_char_size);
        for (size_t i = 0; i < terminal_font_char_size; i++) { cursor_full[i] = CURSOR_FULL_VALUE; }
    }
    cursor_enabled = true;
}

void terminal_cursor_disable() { cursor_enabled = false; }

void terminal_cursor_left()
{
    if (cursor_enabled)
    {
        screen_fill(terminal_xpixel(cursor_x) - 1, terminal_ypixel(cursor_y), 1, terminal_font_char_size, colours[terminal_bg_colour]);
        cursor_active = false;
    }

    if (cursor_x == 0 && cursor_y == 0 && display_type == DISPLAY_RGB) { terminal_rgb_scroll_up(); }
    text_offset--;
    terminal_rebase(text_offset);
}