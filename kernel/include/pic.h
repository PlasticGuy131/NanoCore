#ifndef PIC_H_INCLUDED
#define PIC_H_INCLUDED 1

#include <stdint.h>

void pic_send_eoi(uint8_t irq);

void pic_disable();
void pic_initialize();

#endif