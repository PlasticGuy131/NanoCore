#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED 1

int serial_initialize();

int serial_received();
char serial_read();

int serial_isempty();
void serial_write(char a);

#endif