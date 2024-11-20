.globl _boot
_start:
hang:
    jmp hang

    .fill 510 - (. - _start), 1, 0
    .word 0xAA55

    jmp 0x1000
