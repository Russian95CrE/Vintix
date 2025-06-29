/* panic.c */

// #include "drivers/serial/serial.h"
// #include "print.h"
// #include "stdarg.h"

// void panic (const char* fmt, ...) {
//     va_list args;
//     va_start(args, fmt);

//     char buf[256];
//     vsnprintf(buf, sizeof(buf), fmt, args);

//     va_end(args);

//     serial_writes(buf);
//     serial_writes("\n");

//     while (1) {
//         __asm__ volatile("hlt");
//     }
// }

void panic (void) {
    // needs a rewrite
}