#ifdef _MSC_VER
#include <vscode.h>
#endif

#include <interrupt.h>
#include <kernel.h>

__attribute__((interrupt)) void interrupt_panic()
{
    kernel_panic("Interrupt Occurred!");
}
