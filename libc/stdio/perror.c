#include <errno.h>
#include <stdio.h>
#include <string.h>

void perror(const char* s)
{
    printf("%s: %s\n", s, strerror(errno));
}