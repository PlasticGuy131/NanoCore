.macro interrupt_no_err isr, w=_wpr
.globl \isr\w
\isr\w:
    pushal
    cld
    call \isr
    popal
    iret
.endm

.macro interrupt isr, w=_wpr
.globl \isr\w
\isr\w:
    pushal
    cld
    call \isr
    popal
    addl $4, %esp
    iret
.endm

interrupt_no_err interrupt_div_zero
interrupt_no_err interrupt_debug
interrupt_no_err interrupt_nmi
interrupt_no_err interrupt_breakpoint
interrupt_no_err interrupt_overflow
interrupt_no_err interrupt_bound
interrupt_no_err interrupt_invalid_opcode
interrupt_no_err interrupt_device
interrupt_no_err interrupt_double

interrupt interrupt_tss
interrupt interrupt_segment
interrupt interrupt_stack_segment
interrupt interrupt_general_protection
interrupt interrupt_page

interrupt_no_err interrupt_floating
interrupt_no_err interrupt_alignment
interrupt_no_err interrupt_machine
interrupt_no_err interrupt_SIMD
interrupt_no_err interrupt_virtualization
interrupt_no_err interrupt_control
interrupt_no_err interrupt_hypervisor
interrupt_no_err interrupt_VMM
interrupt_no_err interrupt_security

/*.globl interrupt_panic_wpr
interrupt_panic_wpr:
    pushal
    cld
    call interrupt_panic
    popal
    iret*/


