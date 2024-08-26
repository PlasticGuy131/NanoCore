load_idt:
    mov [%esp+4], %eax
    lidt %eax