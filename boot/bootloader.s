.globl _boot
_boot:
    .fill 510 - (. - _boot), 1, 0
    .word 0xAA55

    jmp 0x200000
