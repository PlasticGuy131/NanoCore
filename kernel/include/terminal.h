#ifndef TERMINAL_H_INCLUDED
#define TERMINAL_H_INCLUDED 1

#include <stddef.h>
#include <stdint.h>

enum colour
{
    COLOUR_BLACK,
    COLOUR_BLUE,
    COLOUR_GREEN,
    COLOUR_CYAN,
    COLOUR_RED,
    COLOUR_MAGENTA,
    COLOUR_BROWN,
    COLOUR_LIGHT_GRAY,
    COLOUR_DARK_GRAY,
    COLOUR_LIGHT_BLUE,
    COLOUR_LIGHT_GREEN,
    COLOUR_LIGHT_CYAN,
    COLOUR_LIGHT_RED,
    COLOUR_LIGHT_MAGENTA,
    COLOUR_LIGHT_BROWN,
    COLOUR_WHITE
};

extern size_t terminal_row;
extern size_t terminal_column;
extern uint8_t terminal_colour;
extern uint16_t* terminal_buffer;

void terminal_initialize(void);
uint8_t terminal_create_colour(enum colour fg, enum colour bg);
void terminal_setcolour(uint8_t colour);
void terminal_putchar(char c);
void terminal_writestring(const char *data);

#endif