#ifndef TERMINAL_H_INCLUDED
#define TERMINAL_H_INCLUDED 1

#include <stddef.h>
#include <stdint.h>

#include <multiboot.h>

enum Colour
{
    COLOUR_BLACK = 0,
    COLOUR_BLUE = 1,
    COLOUR_GREEN = 2,
    COLOUR_CYAN = 3,
    COLOUR_RED = 4,
    COLOUR_MAGENTA = 5,
    COLOUR_BROWN = 6,
    COLOUR_LIGHT_GRAY = 7,
    COLOUR_DARK_GRAY = 8,
    COLOUR_LIGHT_BLUE = 9,
    COLOUR_LIGHT_GREEN = 10,
    COLOUR_LIGHT_CYAN = 11,
    COLOUR_LIGHT_RED = 12,
    COLOUR_LIGHT_MAGENTA = 13,
    COLOUR_LIGHT_BROWN = 14,
    COLOUR_WHITE = 15
};

void terminal_clear();

void terminal_col_default();
void terminal_col_warning();
void terminal_col_error();

void terminal_initialize(Multiboot_Info* multiboot_info);

void terminal_set_colour(enum Colour fg, enum Colour bg);
void terminal_set_default_colour(enum Colour fg, enum Colour bg);
void terminal_set_warning_colour(enum Colour fg, enum Colour bg);
void terminal_set_error_colour(enum Colour fg, enum Colour bg);

void terminal_putchar(char c);

#endif