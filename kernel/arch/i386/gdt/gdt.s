gdtr:
    .word 0
    .long 0

.globl set_gdt
set_gdt:
    /mov $0, %eax
    /mov %ds, %ax
    /shl $4, %eax
    /add _gdt_start, %eax
    mov _gdt_start, %eax
    mov _gdt_start, %edx
    mov %eax, [gdtr+2]
    mov _gdt_end, %eax
    sub _gdt_start, %eax
    dec %eax
    mov %ax, gdtr
    mov $gdtr, %eax
    lgdt [gdtr]
    hlt
