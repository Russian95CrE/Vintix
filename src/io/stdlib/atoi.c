/* atoi.c */

#include "ctype.h"
#include "errno.h"
#include "limits.h"

int atoi (const char* s) {
        long result = 0;
        int  sign   = 1;

        errno = 0;

        while (isspace(*s))
                s++;

        if (*s == '+' || *s == '-') {
                if (*s == '-')
                        sign = -1;
                s++;
        }

        if (!isdigit(*s)) {
                errno = EINVAL;
                return 0;
        }

        while (isdigit(*s)) {
                int digit = *s - '0';

                if (result > (LONG_MAX - digit) / 10) {
                        errno = ERANGE;
                        return sign == 1 ? INT_MAX : INT_MIN;
                }

                result = result * 10 + digit;
                s++;
        }

        result *= sign;

        if (result < INT_MIN || result > INT_MAX) {
                errno = ERANGE;
                return result > 0 ? INT_MAX : INT_MIN;
        }

        return (int) result;
}
