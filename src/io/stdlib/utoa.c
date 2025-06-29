/* utoa.c */

#include "utoa.h"

static const char digits_lower[] = "0123456789abcdef";
static const char digits_upper[] = "0123456789ABCDEF";

char* _utoa (
    char* buf, char* end, unsigned long value, int base, int uppercase) {
        const char* digits = uppercase ? digits_upper : digits_lower;
        char*       rev    = buf;

        if (value == 0) {
                if (buf < end) {
                        *rev++ = '0';
                }
                return rev;
        }

        while (value != 0 && rev < end) {
                *rev++ = digits[(unsigned) (value % (unsigned) base)];
                value /= (unsigned) base;
        }

        // Reverse string
        for (char *p1 = buf, *p2 = rev - 1; p1 < p2; p1++, p2--) {
                char tmp = *p1;
                *p1      = *p2;
                *p2      = tmp;
        }

        return rev;
}