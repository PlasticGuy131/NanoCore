.globl interrupt_panic_wpr
interrupt_panic_wpr:
    .long 7
    //hlt
    //pushal
    //cld
    //call interrupt_panic
    //popal
    //iret
