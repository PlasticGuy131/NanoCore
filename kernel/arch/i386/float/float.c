#include <stdint.h>

#include <float.h>

extern uint32_t fpu_test;

int float_initialize()
{
    if (fpu_test == 0)
    {   
        terminal_col_error();
        printf("ERROR: FLOAT INITIALIZATION FAILED\n");
        terminal_col_default();
        return -1;
    }
    return 0;
}