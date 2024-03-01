#include <stddef.h>
#include <stdint.h>

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