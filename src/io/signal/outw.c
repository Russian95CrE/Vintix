/* outw.c */

void outw (unsigned short port, unsigned short val) {
    __asm__ __volatile__("outw %0, %1" : : "a"(val), "Nd"(port));
}
