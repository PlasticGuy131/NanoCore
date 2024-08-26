.align 4
.globl interrupt_panic_wpr
interrupt_panic_wpr:
    pushad
    cld
    call interrupt_panic
    popad
    iret
    