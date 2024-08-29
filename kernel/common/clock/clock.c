#include <clock.h>

static int clock = 0;

void clock_increment() { clock++; }
int clock_count() { return clock; }