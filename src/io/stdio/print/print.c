/* print.c */

#include "core/kernel.h"
#include "drivers/serial/serial.h"
#include "stdarg.h"
#include "stdio.h"
#include "utoa.h"

typedef __builtin_va_list my_va_list;
#define my_va_start(ap, last) __builtin_va_start(ap, last)
#define my_va_arg(ap, type)   __builtin_va_arg(ap, type)
#define my_va_end(ap)         __builtin_va_end(ap)

// Simple (and still unsafe) way to put HEX.
void puthex (uint64_t n) {
        const char* hex = "0123456789ABCDEF";
        char        buf[17];
        int         i = 16;
        buf[i--]      = '\0';

        do {
                buf[i--] = hex[n & 0xF];
                n >>= 4;
        } while (n > 0);

        puts(&buf[i + 1]);
}

void putdec (uint32_t n) {
        if (n == 0) {
                putchar('0');
                return;
        }

        char buf[11];
        int  i = 0;

        while (n > 0) {
                buf[i++] = (char) ('0' + (n % 10));
                n /= 10;
        }

        while (--i >= 0) {
                putchar(buf[i]);
        }
}

void puts (const char* s) {
        video_puts(s);
        putchar('\n');
}

void duts (const char* s) {
        if (!use_debug) {
                return;
        }
        serial_writes(s);
        serial_write('\n');
}

int vsnprintf (char* buffer, size_t size, const char* format, va_list args) {
        char* out = buffer;
        char* end = buffer + size - 1;

        for (const char* p = format; *p && out < end; ++p) {
                if (*p != '%') {
                        *out++ = *p;
                        continue;
                }
                ++p;

                int left_align = 0;
                if (*p == '-') {
                        left_align = 1;
                        ++p;
                }
                int width = 0;
                while (*p >= '0' && *p <= '9') {
                        width = width * 10 + (*p++ - '0');
                }

                char  temp[32];
                char* t = temp;

                switch (*p) {
                        case 's': {
                                const char* s = va_arg(args, const char*);
                                while (*s && t < temp + sizeof(temp) - 1) {
                                        *t++ = *s++;
                                }
                                break;
                        }
                        case 'c': {
                                *t++ = (char) va_arg(args, int);
                                break;
                        }
                        case 'd': {
                                int          val = va_arg(args, int);
                                unsigned int uval;
                                if (val < 0) {
                                        *t++ = '-';
                                        uval = (unsigned int) (-val);
                                } else {
                                        uval = (unsigned int) val;
                                }
                                t = _utoa(
                                    t, temp + sizeof(temp) - 1, uval, 10, 0);
                                break;
                        }
                        case 'u': {
                                unsigned int uval = va_arg(args, unsigned int);
                                t                 = _utoa(
                                    t, temp + sizeof(temp) - 1, uval, 10, 0);
                                break;
                        }
                        case 'x': {
                                unsigned int uval = va_arg(args, unsigned int);
                                t                 = _utoa(
                                    t, temp + sizeof(temp) - 1, uval, 16, 0);
                                break;
                        }
                        case 'X': {
                                unsigned int uval = va_arg(args, unsigned int);
                                t                 = _utoa(
                                    t, temp + sizeof(temp) - 1, uval, 16, 1);
                                break;
                        }
                        case '%': {
                                *t++ = '%';
                                break;
                        }
                        default: {
                                *t++ = '%';
                                *t++ = *p;
                                break;
                        }
                }
                ++p;

                size_t len = (size_t) (t - temp);
                int    pad = width > (int) len ? width - (int) len : 0;
                if (!left_align) {
                        while (pad-- > 0 && out < end)
                                *out++ = ' ';
                }
                for (size_t i = 0; i < len && out < end; ++i) {
                        *out++ = temp[i];
                }
                if (left_align) {
                        while (pad-- > 0 && out < end)
                                *out++ = ' ';
                }
                --p;
        }

        *out = '\0';
        return (int) (out - buffer);
}

int printf (const char* format, ...) {
        my_va_list args;
        my_va_start(args, format);
        int count = 0;

        for (const char* p = format; *p; ++p) {
                if (*p != '%') {
                        putchar(*p);
                        count++;
                        continue;
                }

                p++;

                // Field width and left alignment
                int left_align = 0;
                int width      = 0;

                if (*p == '-') {
                        left_align = 1;
                        p++;
                }

                while (*p >= '0' && *p <= '9') {
                        width = width * 10 + (*p - '0');
                        p++;
                }

                switch (*p) {
                        case 's': {
                                const char* s   = my_va_arg(args, const char*);
                                int         len = 0;
                                const char* t   = s;
                                while (*t++)
                                        len++;

                                int pad = (width > len) ? (width - len) : 0;

                                if (!left_align) {
                                        for (int i = 0; i < pad; ++i) {
                                                putchar(' ');
                                                count++;
                                        }
                                }

                                for (int i = 0; i < len; ++i) {
                                        putchar(s[i]);
                                        count++;
                                }

                                if (left_align) {
                                        for (int i = 0; i < pad; ++i) {
                                                putchar(' ');
                                                count++;
                                        }
                                }
                                break;
                        }

                        case 'd': {
                                int n = my_va_arg(args, int);
                                if (n < 0) {
                                        putchar('-');
                                        count++;
                                        n = -n;
                                }

                                putdec((uint32_t) n);

                                int temp = n, digits = 1;
                                while (temp >= 10) {
                                        temp /= 10;
                                        digits++;
                                }
                                count += digits;
                                break;
                        }

                        case 'x': {
                                unsigned int n = my_va_arg(args, unsigned int);
                                puthex((uint64_t) n);

                                unsigned int temp   = n;
                                int          digits = 1;
                                while (temp >= 16) {
                                        temp /= 16;
                                        digits++;
                                }
                                count += digits;
                                break;
                        }

                        case 'c': {
                                char c = (char) my_va_arg(args, int);
                                putchar(c);
                                count++;
                                break;
                        }

                        case '%': {
                                putchar('%');
                                count++;
                                break;
                        }

                        case 'u': {
                                unsigned int n = my_va_arg(args, unsigned int);
                                putdec(n);

                                unsigned int temp   = n;
                                int          digits = 1;
                                while (temp >= 10) {
                                        temp /= 10;
                                        digits++;
                                }
                                count += digits;
                                break;
                        }

                        default:
                                putchar('%');
                                putchar(*p);
                                count += 2;
                                break;
                }
        }

        my_va_end(args);
        return count;
}

int debugf (const char* format, ...) {
        if (!use_debug)
                return 0;

        my_va_list args;
        my_va_start(args, format);
        int count = 0;

        for (const char* p = format; *p; ++p) {
                if (*p != '%') {
                        serial_write(*p);
                        count++;
                        continue;
                }

                p++;

                int left_align = 0;
                int width      = 0;

                if (*p == '-') {
                        left_align = 1;
                        p++;
                }

                while (*p >= '0' && *p <= '9') {
                        width = width * 10 + (*p - '0');
                        p++;
                }

                switch (*p) {
                        case 's': {
                                const char* s   = my_va_arg(args, const char*);
                                int         len = 0;
                                const char* t   = s;
                                while (*t++)
                                        len++;

                                int pad = (width > len) ? (width - len) : 0;

                                if (!left_align) {
                                        for (int i = 0; i < pad; ++i) {
                                                serial_write(' ');
                                                count++;
                                        }
                                }

                                serial_writes(s);
                                count += len;

                                if (left_align) {
                                        for (int i = 0; i < pad; ++i) {
                                                serial_write(' ');
                                                count++;
                                        }
                                }
                                break;
                        }

                        case 'd': {
                                int   n = my_va_arg(args, int);
                                char  buf[12];
                                char* ptr = buf;

                                if (n < 0) {
                                        *ptr++ = '-';
                                        n      = -n;
                                }

                                ptr  = _utoa(ptr,
                                            buf + sizeof(buf) - 1,
                                            (unsigned int) n,
                                            10,
                                            0);
                                *ptr = '\0';
                                serial_writes(buf);
                                count += (int) (ptr - buf);
                                break;
                        }

                        case 'x': {
                                unsigned int n = my_va_arg(args, unsigned int);
                                char         buf[12];
                                char*        ptr =
                                    _utoa(buf, buf + sizeof(buf) - 1, n, 16, 0);
                                *ptr = '\0';
                                serial_writes(buf);
                                count += (int) (ptr - buf);
                                break;
                        }

                        case 'c': {
                                char c = (char) my_va_arg(args, int);
                                serial_write(c);
                                count++;
                                break;
                        }

                        case '%': {
                                serial_write('%');
                                count++;
                                break;
                        }

                        default: {
                                serial_write('%');
                                serial_write(*p);
                                count += 2;
                                break;
                        }
                }
        }

        my_va_end(args);
        return count;
}
