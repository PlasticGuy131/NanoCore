.section .text
.globl _start
_start:
hang:
    jmp hang

    .fill 510 - (. - _start), 1, 0
    .word 0xAA55
