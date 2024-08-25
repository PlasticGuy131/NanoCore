#include <stdint.h>
#include <stdlib.h>

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0x55ebc119
#else
#define STACK_CHK_GUARD 0x4a3809b5bc9e66d2
#endif

#ifdef __is_libk
#include <kernel.h>
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn)) void __stack_chk_fail(void)
{
#ifdef __is_libk
    kernel_panic("Stack Smash Detected.");
#else
    abort();
#endif
}