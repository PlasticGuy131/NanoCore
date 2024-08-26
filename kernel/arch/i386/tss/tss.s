.globl load_tss
load_tss:
    mov $0x28, %ax
    ltr %ax
    ret