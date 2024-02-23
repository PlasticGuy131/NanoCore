#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Colour constants. */
enum vga_colour
{
    VGA_COLOUR_BLACK = 0,
    VGA_COLOUR_BLUE = 1,
    VGA_COLOUR_GREEN = 2,
    VGA_COLOUR_CYAN = 3,
    VGA_COLOUR_RED = 4,
    VGA_COLOUR_MAGENTA = 5,
    VGA_COLOUR_BROWN = 6,
    VGA_COLOUR_LIGHT_GRAY = 7,
    VGA_COLOUR_DARK_GRAY = 8,
    VGA_COLOUR_LIGHT_BLUE = 9,
    VGA_COLOUR_LIGHT_GREEN = 10,
    VGA_COLOUR_LIGHT_CYAN = 11,
    VGA_COLOUR_LIGHT_RED = 12,
    VGA_COLOUR_LIGHT_MAGENTA = 13,
    VGA_COLOUR_LIGHT_BROWN = 14,
    VGA_COLOUR_WHITE = 15
};

static inline uint8_t vga_entry_colour(enum vga_colour fg, enum vga_colour bg)
{
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t colour)
{
    return (uint16_t)uc | (uint16_t)colour << 8;
}

size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len]) { len++; }
    return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static const char* VERSION = "N/A";

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_colour;
uint16_t* terminal_buffer;

void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_colour = vga_entry_colour(VGA_COLOUR_LIGHT_GRAY, VGA_COLOUR_BLACK);
    terminal_buffer = (uint16_t *)0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_colour);
        }
    }
}

void terminal_setcolour(uint8_t colour)
{
    terminal_colour = colour;
}

void terminal_putentryat(char c, uint8_t colour, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, colour);
}

void terminal_putchar(char c)
{
    switch (c)
    {
        case '\n':
            terminal_column = 0;
            if (++terminal_row >= VGA_HEIGHT) { terminal_row = 0; }
            break;
        default:
            terminal_putentryat(c, terminal_colour, terminal_column, terminal_row);
            if(++terminal_column >= VGA_WIDTH)
            {
                terminal_column = 0;
                if (++terminal_row >= VGA_HEIGHT) { terminal_row = 0; }
            }
    }
}

void terminal_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++) { terminal_putchar(data[i]); }
}

void terminal_writestring(const char* data)
{
    terminal_write(data, strlen(data));
}

void terminal_intro_splash()
{
    terminal_setcolour(vga_entry_colour(VGA_COLOUR_RED, VGA_COLOUR_BLACK));
    terminal_writestring("NANO/CORE/NANO/CORE/\n");
    terminal_setcolour(vga_entry_colour(VGA_COLOUR_GREEN, VGA_COLOUR_BLACK));
    terminal_writestring("C/OREN/ANOC/OREN/ANO\n");
    terminal_setcolour(vga_entry_colour(VGA_COLOUR_CYAN, VGA_COLOUR_BLACK));
    terminal_writestring("NA/NOCO/RENA/NOCO/RE\n");
    terminal_setcolour(vga_entry_colour(VGA_COLOUR_BLUE, VGA_COLOUR_BLACK));
    terminal_writestring("COR/ENAN/OCOR/ENAN/O\n");
    terminal_setcolour(vga_entry_colour(VGA_COLOUR_WHITE, VGA_COLOUR_BLACK));
    terminal_writestring("NANO CORE v.");
    terminal_writestring(VERSION);
    terminal_writestring("\nWelcome.\n");
}

void kernel_main(void)
{
    terminal_initialize();

    terminal_intro_splash();
    terminal_writestring("Testing scrolling:");
    for (size_t i = 0; i < 100; i++)
    {
        char str[4] = " N\n";
        int out = i;
        if (i >= 10)
        {
            int tens = i / 10;
            str[0] = tens + '0';
            out -= tens * 10;
        }
        str[1] = out + '0';
        terminal_writestring(str);
    }
}
