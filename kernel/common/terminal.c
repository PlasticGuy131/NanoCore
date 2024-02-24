#include <stddef.h>
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