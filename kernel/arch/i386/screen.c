#include <stdint.h>

#include <multiboot.h>

struct Colour{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

void screen_putpixel(int x, int y, struct Colour colour, Multiboot_Info* multiboot_info)
{
    char* screen = (char*)multiboot_info->framebuffer_addr;
    unsigned where = x * (multiboot_info->framebuffer_bpp / 8) + y * multiboot_info->framebuffer_pitch;
    screen[where+0] = colour.r;
    screen[where+1] = colour.g;
    screen[where+2] = colour.b;
}

struct Colour screen_colour_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    struct Colour colour;
    colour.r = r;
    colour.g = g;
    colour.b = b;
    return colour;
}