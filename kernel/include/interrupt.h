#ifndef INTERRUPT_H_INCLUDED
#define INTERRUPT_H_INCLUDED 1

/// @brief Initialises interrupts
/// @return Status Code: 0 OK, 1 ERROR
int interrupt_initialize();

/// @brief Interrupt that panics kernel
//void interrupt_panic_wpr();

#endif