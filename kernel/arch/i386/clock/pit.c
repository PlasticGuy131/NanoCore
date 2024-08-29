#include <interrupt.h>
#include <port.h>

static const int PIT_COUNT_DEFAULT = 1194;

void pit_set_count(unsigned count)
{
    disable_interrupts();

    outb(PIT0, count & 0xFF);
    outb(PIT0, (count >> 8) & 0xFF);

    enable_interrupts();
}