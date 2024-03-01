#include <stdint.h>

#include <multiboot.h>
#include <terminal.h>

struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct RGB colours[16];

char* screen;
int screen_pixel_width;
int screen_pitch;

struct RGB screen_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    struct RGB colour;
    colour.r = r;
    colour.g = g;
    colour.b = b;
    return colour;
}

void screen_initialize(Multiboot_Info* multiboot_info)
{
    screen = (char*)multiboot_info->framebuffer_addr;
    screen_pixel_width = multiboot_info->framebuffer_bpp / 8;
    screen_pitch = multiboot_info->framebuffer_pitch;

    colours[0] = screen_rgb(0x00, 0x00, 0x00);
    colours[1] = screen_rgb(0x43, 0x43, 0x6A);
    colours[2] = screen_rgb(0x8A, 0xB0, 0x60);
    colours[3] = screen_rgb(0x68, 0xC2, 0xD3);
    colours[4] = screen_rgb(0xB4, 0x52, 0x52);
    colours[5] = screen_rgb(0x6A, 0x53, 0x6E);
    colours[6] = screen_rgb(0x80, 0x49, 0x3A);
    colours[7] = screen_rgb(0xB8, 0xB5, 0xB9);
    colours[8] = screen_rgb(0x64, 0x63, 0x65);
    colours[9] = screen_rgb(0x4B, 0x80, 0xCA);
    colours[10] = screen_rgb(0xC2, 0xD3, 0x68);
    colours[11] = screen_rgb(0xA2, 0xDC, 0xC7);
    colours[12] = screen_rgb(0xED, 0xC8, 0xC4);
    colours[13] = screen_rgb(0xCF, 0x8A, 0xCB);
    colours[14] = screen_rgb(0xA7, 0x7B, 0x5B);
    colours[15] = screen_rgb(0xFF, 0xFF, 0xFF);
}

static inline unsigned screen_coords(int x, int y)
{
    return x * screen_pixel_width + y * screen_pitch;
}

void screen_putpixel(int x, int y, struct RGB colour)
{
    unsigned where = screen_coords(x, y);
    screen[where+0] = colour.r;
    screen[where+1] = colour.g;
    screen[where+2] = colour.b;
}

void screen_putbitmap_bw(int left, int top, uint8_t* start, int width_bytes, int height_pixels, struct RGB fg, struct RGB bg)
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

struct RGB screen_rgb_name(enum Colour colour)
{
    return colours[colour];
}