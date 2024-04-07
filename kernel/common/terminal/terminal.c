#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <terminal.h>

extern enum Colour terminal_fg_colour;
extern enum Colour terminal_bg_colour;

extern enum Colour terminal_default_fg_colour;
extern enum Colour terminal_default_bg_colour;

extern enum Colour terminal_warning_fg_colour;
extern enum Colour terminal_warning_bg_colour;

extern enum Colour terminal_error_fg_colour;
extern enum Colour terminal_error_bg_colour;

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