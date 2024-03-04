#ifndef PORT_H_INCLUDED
#define PORT_H_INCLUDED 1

#include <stdint.h>

static const uint32_t BDA_START = 0x0400;

void outb(uint16_t port, uint8_t val);
void outw(uint16_t port, uint16_t val);
void outl(uint16_t port, uint32_t val);

uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t inl(uint16_t port);

uint32_t port_get_com1();

#endif