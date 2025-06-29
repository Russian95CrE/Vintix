/* strnlen.c */

#include "stddef.h"
#include "strnlen.h"

size_t strnlen (const char* str, size_t maxlen) {
        size_t len = 0;
        while (len < maxlen && str[len] != '\0') {
                len++;
        }
        return len;
}
