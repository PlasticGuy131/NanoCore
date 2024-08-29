ENTRY(_start)

SECTIONS
{
    /* Define start offset */
    . = 2M;

    /* Multiboot data must be early, and then text */
    .text BLOCK(8k) : ALIGN(4k)
    {
        *(.multiboot)
        *(.text)
    }

    /* Read-only data */
    .rodata BLOCK(8k) : ALIGN(4k)
    {
        *(.rodata)
    }

    /* Read/Write data (initialised) */
    .data BLOCK(8k) : ALIGN(4k)
    {
        *(.data)
    }

    /* Read/Write data (unititialised) and stack */
    .bss BLOCK(8k) : ALIGN(4k)
    {
        *(COMMON)
        *(.bss)
    }
}