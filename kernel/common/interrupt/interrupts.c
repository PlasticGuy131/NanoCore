#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <clock.h>
#include <interrupt.h>
#include <kernel.h>
#include <pic.h>

void (*interrupt_callback)();

static bool try_return()
{
    if (interrupt_callback)
    {
        interrupt_callback();
        return true;
    }
    else { return false; }
}

void interrupt_register_callback(void (*callback)()) { interrupt_callback = callback; }
void interrupt_end_callback() { interrupt_callback = 0; }

void interrupt_div_zero() { if (!try_return()) { kernel_panic("Exception Occurred: Division Error"); } }
void interrupt_debug() { kernel_panic("Exception Occurred: Debug (not supported)"); }
void interrupt_nmi() { kernel_panic("Exception Occurred: Hardware Error"); }
void interrupt_breakpoint() { kernel_panic("Exception Occurred: Breakpoint (not supported)"); }
void interrupt_overflow() { if (!try_return()) { kernel_panic("Exception Occurred: Overflow Error"); } }
void interrupt_bound() { if (!try_return()) { kernel_panic("Exception Occurred: Bound range exceeded"); } }
void interrupt_invalid_opcode() { if (!try_return()) { kernel_panic("Exception Occurred: Invalid Opcode"); } }
void interrupt_device() { if (!try_return()) { kernel_panic("Exception Occurred: Device not available"); } }
void interrupt_double() { kernel_panic("DOUBLE FAULT OCCURRED: UNRECOVERABLE"); }

void interrupt_page(uint32_t error)
{
    char buf[40];
    sprintf(buf, "Exception Occurred: Page Fault (%#.4x)", error);
    kernel_panic(buf);
}

void interrupt_floating() { kernel_panic("Exception Occurred: x86 Floating Point Exception"); }
void interrupt_alignment() { kernel_panic("Exception Occurred: Alignment Check Failure"); }
void interrupt_machine() { kernel_panic("Exception Occurred: Machine Check Failure"); }
void interrupt_SIMD() { kernel_panic("Exception Occurred: SIMD Floating Point Exception"); }
void interrupt_virtualization() { kernel_panic("Exception Occurred: Virtualization Exception"); }
void interrupt_control() { kernel_panic("Exception Occurred: Control protection Exception"); }
void interrupt_hypervisor() { kernel_panic("Exception Occurred: Hypervisior Injection Exception"); }
void interrupt_VMM() { kernel_panic("Exception Occurred: VMM Communication Exception"); }
void interrupt_security() { kernel_panic("Exception Occurred: Security Exception"); }

void interrupt_clock()
{
    clock_increment();
    pic_send_eoi();
}

void interrupt_keyboard()
{
    printf("Keyboard Interrupt\n");
    pic_send_eoi();
}
