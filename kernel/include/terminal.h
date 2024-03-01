#ifndef TERMINAL_H_INCLUDED
#define TERMINAL_H_INCLUDED 1

#include <stddef.h>
#include <stdint.h>

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

extern uint32_t multiboot_magic;
extern uint32_t multiboot_info_start;

void terminal_initialize(void);
void terminal_set_colour(enum Colour fg, enum Colour bg);
void terminal_putchar(char c);
void terminal_writestring(const char *data);
void terminal_writeint(int n);
void terminal_writebyte(unsigned char b);

#endif