#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <terminal.h>

void terminal_set_colour(enum Colour fg, enum Colour bg)
{
    terminal_fg_colour = fg;
    terminal_bg_colour = bg;
}