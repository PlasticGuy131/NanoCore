#include <stdint.h>
#include <stdio.h>

#include <float_init.h>
#include <terminal.h>

extern uint32_t _fpu_test;

int float_initialize() { return _fpu_test; }