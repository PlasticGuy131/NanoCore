#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <terminal.h>

static void terminal_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++) { terminal_putchar(data[i]); }
}

void terminal_set_colour(enum Colour fg, enum Colour bg)
{
    terminal_fg_colour = fg;
    terminal_bg_colour = bg;
}