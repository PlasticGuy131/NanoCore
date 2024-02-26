#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <terminal.h>

static void terminal_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++) { terminal_putchar(data[i]); }
}

void terminal_writestring(const char* data)
{
    terminal_write(data, strlen(data));
}

void terminal_writeint(int n)
{
    size_t len = 1;
    uint32_t header = 1;
    while (n / header >= 10)
    {
        header *= 10;
        len++;
    }

    for (size_t i = 0; i < len; i++)
    {
        char c = (char)(n / header);
        terminal_putchar(c + '0');
        n %= header;
        header /= 10;
    }
}

void terminal_writebyte(unsigned char b)
{
    unsigned char mask = 1;
    mask << 7;
    for (size_t i = 0; i < 8; i++)
    {
        terminal_putchar((b & mask) ? '1' : '0');
    }
}