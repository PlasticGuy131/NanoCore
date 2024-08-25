gdtr:
    .word 0
    .long 0

.globl set_gdt
set_gdt:
    mov $_gdt_start, %eax
    mov %eax, gdtr+2
    mov $_gdt_end, %eax
    sub $_gdt_start, %eax
    mov %eax, gdtr
    lgdt gdtr
