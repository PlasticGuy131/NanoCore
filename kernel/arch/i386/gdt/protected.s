.globl enter_protected
enter_protected:
    cli
    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0

    hlt
    ljmp $0x08, $.reload_CS
.reload_CS:
    hlt
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    hlt
    ret
