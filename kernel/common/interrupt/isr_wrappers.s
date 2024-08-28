.macro interrupt_no_err isr, w=_wpr
.globl \isr\w
\isr\w:
    pushal
    cld
    call \isr
    popal
    iret
.endm

interrupt_no_err interrupt_panic

/*.globl interrupt_panic_wpr
interrupt_panic_wpr:
    pushal
    cld
    call interrupt_panic
    popal
    iret*/


