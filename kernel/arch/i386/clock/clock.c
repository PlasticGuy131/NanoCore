#include <stdio.h>

#include <clock.h>

#include "pit.c"

void clock_initialize() { pit_set_count(PIT_COUNT_DEFAULT); }

void clock_increment()
{
    printf(".");
}