ENTRY(_start)

SECTIONS
{
    / * Define start offset * /
    . = 2M;

    / * Multiboot data must be early, and then text * /
    .text BLOCK(4k) : ALIGN(4k)
    {
        *(.multiboot)
        *(.text)
    }

    / * Read-only data * /
    .rodata BLOCK(4k) : ALIGN(4k)
    {
        *(.rodata)
    }

    / * Read/Write data (initialised) * /
    .data BLOCK(4k) : ALIGN(4k)
    {
        *(.data)
    }

    / * Read/Write data (unititialised) and stack * /
    .bss BLOCK(4k) : ALIGN(4k)
    {
        *(COMMON)
        *(.bss)
    }
}