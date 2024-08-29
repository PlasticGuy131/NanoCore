#include <stdio.h>

#include <clock.h>

static int clock = 0;

void clock_increment()
{
    printf(".");
    clock++;
}

int clock_count() { return clock; }