#include <stdio.h>

#include <clock.h>
#include <interrupt.h>

#include "pit.c"

static volatile unsigned timer = 0;

void clock_initialize() { pit_set_count(PIT_COUNT_DEFAULT); }

void clock_increment() { if (timer) { timer--; } }

void clock_sleep(unsigned time)
{
    //pic_unmask_irq(PIC_PIT);
    timer = time;
    while (timer);
    //pic_mask_irq(PIC_PIT);
}