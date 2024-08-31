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

/// @brief Clears the terminal
void terminal_clear();

/// @brief Sets the terminal to its default colour
void terminal_col_default();
/// @brief Sets the terminal to its warning colour
void terminal_col_warning();
/// @brief Sets the terminal to its error colour
void terminal_col_error();

/// @brief Initializes the terminal
/// @param multiboot_info Multiboot Information Data Structure
void terminal_initialize(Multiboot_Info* multiboot_info);

/// @brief Sets the current terminal colour
/// @param fg Foreground colour
/// @param bg Background colour
void terminal_set_colour(enum Colour fg, enum Colour bg);
/// @brief Sets the default terminal colour
/// @param fg Foreground colour
/// @param bg Background colour
void terminal_set_default_colour(enum Colour fg, enum Colour bg);
/// @brief Sets the terminal warning colour
/// @param fg Foreground colour
/// @param bg Background colour
void terminal_set_warning_colour(enum Colour fg, enum Colour bg);
/// @brief Sets the terminal error colour
/// @param fg Foreground colour
/// @param bg Background colour
void terminal_set_error_colour(enum Colour fg, enum Colour bg);

/// @brief Writes a chacter to the terminal
/// @param c Character to write
void terminal_putchar(int c);

#endif