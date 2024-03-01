#include <stdint.h>

#include <multiboot.h>
#include <terminal.h>

struct RBG{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

static const struct RGB colours[16] = {
    screen_rgb(0x00, 0x00, 0x00),
    screen_rgb(0x43, 0x43, 0x6A),
    screen_rgb(0x8A, 0xB0, 0x60),
    screen_rgb(0x68, 0xC2, 0xD3),
    screen_rgb(0xB4, 0x52, 0x52),
    screen_rgb(0x6A, 0x53, 0x6E),
    screen_rgb(0x80, 0x49, 0x3A),
    screen_rgb(0xB8, 0xB5, 0xB9),
    screen_rgb(0x64, 0x63, 0x65),
    screen_rgb(0x4B, 0x80, 0xCA),
    screen_rgb(0xC2, 0xD3, 0x68),
    screen_rgb(0xA2, 0xDC, 0xC7),
    screen_rgb(0xED, 0xC8, 0xC4),
    screen_rgb(0xCF, 0x8A, 0xCB),
    screen_rgb(0xA7, 0x7B, 0x5B),
    screen_rgb(0xFF, 0xFF, 0xFF),
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

void screen_putbitmap_bw(int left, int top, uint8_t* start, int width_bytes, int height_pixels, struct Colour fg, struct Colour bg)
{
    unsigned where = screen_coords(left, top);
    for (int y = 0; y < height_pixels; y++)
    {
        for (int x = 0; x < width_bytes; x++)
        {
            uint8_t mask = 1 << 7;
            for (int i = 0; i < 8; i++)
            {
                uint8_t value = mask & start[x+width_bytes*y];
                screen[where+0] = value ? fg.r : bg.r;
                screen[where+1] = value ? fg.g : bg.g;
                screen[where+2] = value ? fg.b : bg.b;
                mask = mask >> 1;
                where += screen_pixel_width;
            }
        }
        where += screen_pitch;
        where -= screen_pixel_width * width_bytes * 8;
    }
}

struct RBG screen_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    struct RBG colour;
    colour.r = r;
    colour.g = g;
    colour.b = b;
    return colour;
}

struct RBG screen_rgb_name(enum Colour colour)
{
    return colours[colour];
}