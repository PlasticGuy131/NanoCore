#ifndef INTERRUPT_H_INCLUDED
#define INTERRUPT_H_INCLUDED 1

/// @brief Initialises interrupts
/// @return Status Code: 0 OK, 1 ERROR
int interrupt_initialize();

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

#endif