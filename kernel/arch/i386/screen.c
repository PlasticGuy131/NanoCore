#include <stdint.h>

#include <multiboot.h>

struct Colour{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

char* screen;
int screen_pixel_width;
int screen_pitch;

static inline unsigned screen_coords(int x, int y)
{
    return x * screen_pixel_width + y * screen_pitch;
}

void screen_initialize(Multiboot_Info* multiboot_info)
{
    screen = (char*)multiboot_info->framebuffer_addr;
    screen_pixel_width = multiboot_info->framebuffer_bpp / 8;
    screen_pitch = multiboot_info->framebuffer_pitch;
}

void screen_putpixel(int x, int y, struct Colour colour)
{
    unsigned where = screen_coords(x, y);
    screen[where+0] = colour.r;
    screen[where+1] = colour.g;
    screen[where+2] = colour.b;
}

void screen_putbitmap_bw(int left, int top, uint8_t* start, int width, int height, struct Colour fg, struct Colour bg)
{
    unsigned where = screen_coords(left, top);
    width = ((width + 7) & ~7) / 8;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint8_t mask = 1 << 7;
            for (int i = 0; i < 8; i++)
            {
                uint8_t value = mask & *start;
                screen[where+0] = value ? fg.r : bg.r;
                screen[where+1] = value ? fg.g : bg.g;
                screen[where+2] = value ? fg.b : bg.b;
                mask >> 1;
                where += screen_pixel_width;
            }
            start++;
        }
        where += screen_pitch;
        where -= screen_pixel_width * width * 8;
    }
}

struct Colour screen_colour_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    struct Colour colour;
    colour.r = r;
    colour.g = g;
    colour.b = b;
    return colour;
}