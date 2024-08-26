.globl idtr
idtr:
    .word 0
    .long 0

load_idt:
    lidt [idtr]
    ret