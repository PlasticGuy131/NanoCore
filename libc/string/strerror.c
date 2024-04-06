#include <errno.h>
#include <string.h>

char* strerror(int errnum)
{
    switch (errnum)
    {
        case ENOMEM:
            return "Out of memory";
        case EOVERFLOW:
            return "Value too large for defined data type";
        default:
            return "Unknown error";
    }
}