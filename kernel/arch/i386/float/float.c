#include <stdint.h>

#include <float.h>

extern uint32_t fpu_test;

int float_initialize()
{
    if (fpu_test == 0) { return -1; }
    return 0;
}