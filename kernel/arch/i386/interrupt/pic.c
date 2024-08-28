#include <stdint.h>

#include <port.h>

static const int PIC_EOI = 0x20;

static const int ICW1_ICW4 = 0x01;
static const int ICW1_INIT = 0x10;

static const int ICW4_8086 = 0x01;

static const int PIC_OFFSET = 0x20;

static void pic_mask_irq(uint8_t irq)
{
    uint16_t port;
    if (irq < 8) { port = PIC1_DATA; }
    else { port = PIC2_DATA; }

    outb(port, inb(port) | (1 << irq));
}

static void pic_unmask_irq(uint8_t irq)
{
    uint16_t port;
    if (irq < 8) { port = PIC1_DATA; }
    else { port = PIC2_DATA; }

    outb(port, inb(port) & ~(1 << irq));
}

void pic_send_eoi(uint8_t irq)
{
    if (irq >= 8) { outb(PIC2_COM, PIC_EOI); }
    outb(PIC1_COM, PIC_EOI);
}

void pic_disable()
{
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void pic_initialize()
{
    pic_disable();

    outb(PIC1_COM, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COM, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, PIC_OFFSET);
    io_wait();
    outb(PIC2_DATA, PIC_OFFSET + 8);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    pic_unmask_irq(0);
    pic_unmask_irq(2);
}