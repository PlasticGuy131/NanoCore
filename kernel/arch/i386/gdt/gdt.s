gdtr:
    .word 0
    .long 0

set_gdt:
    mov $gdt, %eax
    mov %eax, gdtr+2
    mov $gdt_end, %eax
    sub $gdt, %eax
    mov %eax, gdtr
    lgdt gdtr
