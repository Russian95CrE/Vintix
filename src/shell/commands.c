/* commands.c */

#include "drivers/video/video.h"
#include "shell/commands.h"
#include "signal.h"
#include "stdio.h"

void clear (const char* hex) {
    if (hex == NULL)
        video_clear(0x000000);
    else
        video_clear(hexstr_to_bgr(hex));
}

void poweroff (void) {
    // ACPI poweroff (works in QEMU, VirtualBox, and some real hardware)
    outw(0x604, 0x2000);
    // Fallback for Bochs/QEMU
    outw(0xB004, 0x2000);
    // Infinite loop if poweroff fails
    while (1) {
        __asm__ volatile("hlt");
    }
}

void reboot (void) {
    // Wait until the keyboard controller is ready.
    while (inb(0x64) & 0x02)
        ;
    // Send the reset command
    outb(0x64, 0xFE);
    // If that fails, halt
    while (1) {
        __asm__ volatile("hlt");
    }
}