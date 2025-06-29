/* cpu_relax.c */

#include "cpu_relax.h"

void cpu_relax () {
        __asm__ volatile("pause");
}