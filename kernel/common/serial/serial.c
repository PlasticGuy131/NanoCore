#include <stdint.h>

#include <port.h>
#include <serial.h>

uint32_t port;

int serial_initialize()
{
    port = port_get_com1();

    outb(port + 1, 0x00); // Disable all interrupts
    outb(port + 3, 0x80); // Enable DLAB
    outb(port + 0, 0x03); // Divisor -> 3 (38400 baud) (lo byte)
    outb(port + 1, 0x00); //                           (hi byte)
    outb(port + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(port + 2, 0xC7); // Enable FIFO
    outb(port + 4, 0x0B); // IRQs enabled, RTS/DSR set
    outb(port + 4, 0x1E); // Set loopback mode
    outb(port + 0, 0xAE); // Sent test data 0xAE

    if(inb(port + 0) != 0xAE) { return 1; }

    outb(port + 4, 0x0F); // Set mode to normal
    return 0;
}

int serial_received()
{
    return inb(port + 5) & 1;
}

char serial_read()
{
    while(serial_received() == 0);

    return inb(port);
}

int serial_isempty()
{
    return inb(port + 5) & 0x20;
}

void serial_write(char a)
{
    while (serial_isempty() == 0);

    outb(port, a);
}