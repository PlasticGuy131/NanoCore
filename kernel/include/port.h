#ifndef PORT_H_INCLUDED
#define PORT_H_INCLUDED 1

#include <stdint.h>

#define WAIT_PORT 0x0080

#define PIC1_COM 0x0020
#define PIC1_DATA 0x0021
#define PIC2_COM 0x00A0
#define PIC2_DATA 0x00A1

#define SYSTEM_CONTROL_A 0x0092
#define SYSTEM_CONTROL_B 0x0061

#define BDA_START 0x0400

/// @brief Writes 1B to a serial port
/// @param port Serial port address
/// @param val Value
void outb(uint16_t port, uint8_t val);
/// @brief Writes 2B to a serial port
/// @param port Serial port address
/// @param val Value
void outw(uint16_t port, uint16_t val);
/// @brief Writes 4B to a serial port
/// @param port Serial port address
/// @param val Value
void outl(uint16_t port, uint32_t val);

/// @brief Reads 1B from a serial port
/// @param port Serial port address
/// @return Read value
uint8_t inb(uint16_t port);

/// @brief Reads 2B from a serial port
/// @param port Serial port address
/// @return Read value
uint16_t inw(uint16_t port);

/// @brief Reads 4B from a serial port
/// @param port Serial port address
/// @return Read value
uint32_t inl(uint16_t port);

/// @brief Waits a small amount of time
void io_wait();

/// @brief Gets the address of Serial Port COM1
/// @return Address of COM1
uint32_t port_get_com1();

#endif