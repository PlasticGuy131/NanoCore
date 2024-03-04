#ifndef PORT_H_INCLUDED
#define PORT_H_INCLUDED 1

#include <stdint.h>

static const uint32_t BDA_START = 0x0400;

inline void outb(uint16_t port, uint8_t val);
inline void outw(uint16_t port, uint16_t val);
inline void outl(uint16_t port, uint32_t val);

inline uint8_t inb(uint16_t port);
inline uint16_t inw(uint16_t port);
inline uint32_t inl(uint16_t port);

uint32_t port_get_com1();

#endif