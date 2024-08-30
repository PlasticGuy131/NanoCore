#include <ctype.h>

int tolower(int ch) { return isupper(ch) ? ch + 32 : ch; }