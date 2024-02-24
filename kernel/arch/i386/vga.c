#include <stddef.h>
#include <stdint.h>

/* Colour constants. */
enum colour
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

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static const char* VERSION = "N/A";

inline uint8_t vga_entry_colour(enum colour fg, enum colour bg)
{
    return fg | bg << 4;
}

inline uint16_t vga_entry(unsigned char uc, uint8_t colour)
{
    return (uint16_t)uc | (uint16_t)colour << 8;
}