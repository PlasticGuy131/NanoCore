#include <interrupt.h>
#include <kernel.h>

void interrupt_panic()
{
    kernel_panic("Interrupt Occurred!");
}
