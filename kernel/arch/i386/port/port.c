#ifdef _MSC_VER
#include <vscode.h>
#endif

#include <stdint.h>

#include <port.h>

inline void outb(uint16_t port, uint8_t val)  { __asm__ volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory"); }
inline void outw(uint16_t port, uint16_t val) { __asm__ volatile("outw %w0, %w1" : : "a"(val), "Nd"(port) : "memory"); }
inline void outl(uint16_t port, uint32_t val) { __asm__ volatile("outl %0, %w1" : : "a"(val), "Nd"(port) : "memory"); }

inline uint8_t inb(uint16_t port) 
{
    uint8_t ret;
    __asm__ volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    __asm__ volatile("inw %w1, %w0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    __asm__ volatile("inl %w1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

uint32_t port_get_com1()
{
    uint32_t* bda = (uint32_t*)BDA_START;
    return *bda;
}