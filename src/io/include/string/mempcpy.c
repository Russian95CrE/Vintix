/* mempcpy.c */

#include "stddef.h"

void* mempcpy (void* dest, const void* src, size_t n) {
    char*       d = (char*) dest;
    const char* s = (const char*) src;
    for (size_t i = 0; i < n; i++)
        d[i] = s[i];
    return d + n;
}
