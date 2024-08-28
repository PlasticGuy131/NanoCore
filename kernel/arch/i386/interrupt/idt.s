.globl idtr
idtr:
    .word 0
    .long 0

.globl load_idt
load_idt:
    lidt [idtr]
    ret

.globl enable_interrupts
enable_interrupts:
    sti
    ret
