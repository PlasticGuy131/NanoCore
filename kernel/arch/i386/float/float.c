#include <stdint.h>
#include <stdio.h>

#include <float.h>
#include <terminal.h>

extern uint32_t _fpu_test;

int float_initialize()
{
    if (_fpu_test != 0)
    {   
        terminal_col_error();
        printf("ERROR: FLOAT INITIALIZATION FAILED\n");
        terminal_col_default();
        return 1;
    }
    return 0;
}