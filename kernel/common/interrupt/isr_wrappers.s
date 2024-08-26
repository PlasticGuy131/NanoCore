.globl interrupt_panic_wpr
interrupt_panic_wpr:
    hlt
    pushal
    cld
    call interrupt_panic
    popal
    iret
