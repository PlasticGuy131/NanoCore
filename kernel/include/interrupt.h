#ifndef INTERRUPT_H_INCLUDED
#define INTERRUPT_H_INCLUDED 1

#include <stdint.h>

#define PIC_PIT 0

extern void (*interrupt_callback)();

/// @brief Initialises interrupts
void interrupt_initialize();

/// @brief Registers a callback function that will be called in case of minor exception, instead of halting the program (Always pair with interrupt_end_callback)
/// @param callback Callback function (used to recover the exception)
void interrupt_register_callback(void (*callback)());

/// @brief Un-registers a callback function so that minor exceptions will halt the program
void interrupt_end_callback();

void interrupt_panic_wpr();
void interrupt_arg_test_wpr(uint32_t arg);

void interrupt_div_zero_wpr();
void interrupt_debug_wpr();
void interrupt_nmi_wpr();
void interrupt_breakpoint_wpr();
void interrupt_overflow_wpr();
void interrupt_bound_wpr();
void interrupt_invalid_opcode_wpr();
void interrupt_device_wpr();
void interrupt_double_wpr();
void interrupt_tss_wpr(uint32_t selector);
void interrupt_segment_wpr(uint32_t selector);
void interrupt_stack_segment_wpr(uint32_t selector);
void interrupt_general_protection_wpr(uint32_t selector);
void interrupt_page_wpr(uint32_t selector);
void interrupt_floating_wpr();
void interrupt_alignment_wpr();
void interrupt_machine_wpr();
void interrupt_SIMD_wpr();
void interrupt_virtualization_wpr();
void interrupt_control_wpr();
void interrupt_hypervisor_wpr();
void interrupt_VMM_wpr();
void interrupt_security_wpr();

void interrupt_clock_wpr();
void interrupt_keyboard_wpr();

void pic_send_eoi(uint8_t irq);
void pic_disable();
void pic_initialize();

void enable_interrupts();
void disable_interrupts();

void pic_mask_irq(uint8_t irq);
void pic_unmask_irq(uint8_t irq);

#endif