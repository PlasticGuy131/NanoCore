#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED 1

/// @brief Initializes COM1 for serial communication
/// @return Status Code: 0 OK, 1 ERROR
int serial_initialize();

/// @brief Read a character from serial
/// @return Read character
char serial_read();

/// @brief Write a character to serial
/// @param a Character to write
void serial_write(char a);

#endif