.globl interrupt_panic_wpr
interrupt_panic_wpr:
    hlt
    ret
    //pushal
    //cld
    //call interrupt_panic
    //popal
    //ret
