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
    addl $4, %esp
    popal
    iret
.endm

.globl interrupt_arg_test_wpr
interrupt_arg_test_wpr:
    pushal
    cld
    push $7
    call interrupt_arg_test
    addl $4, %esp
    popal
    iret

interrupt_no_err interrupt_panic
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


