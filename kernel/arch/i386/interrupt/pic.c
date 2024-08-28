#include <port.h>

void pic_disable()
{
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}