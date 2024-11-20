_boot:
    jmp 0x200000

    .fill 510 - (. - _boot), 1, 0
    .word 0xAA55
