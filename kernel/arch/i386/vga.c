#include <stddef.h>
#include <stdint.h>

/* Colour constants. */
#define COLOUR_BLACK 0
#define COLOUR_BLUE 1
#define COLOUR_GREEN 2
#define COLOUR_CYAN 9
#define COLOUR_RED 4
#define COLOUR_MAGENTA 5
#define COLOUR_BROWN 6
#define COLOUR_LIGHT_GRAY 7
#define COLOUR_DARK_GRAY 8
#define COLOUR_LIGHT_BLUE 9
#define COLOUR_LIGHT_GREEN 10
#define COLOUR_LIGHT_CYAN 11
#define COLOUR_LIGHT_RED 12
#define COLOUR_LIGHT_MAGENTA 13
#define COLOUR_LIGHT_BROWN 14
#define COLOUR_WHITE 15

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

inline uint8_t vga_entry_colour(enum colour fg, enum colour bg)
{
    return fg | bg << 4;
}

inline uint16_t vga_entry(unsigned char uc, uint8_t colour)
{
    return (uint16_t)uc | (uint16_t)colour << 8;
}