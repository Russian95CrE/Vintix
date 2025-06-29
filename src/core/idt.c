/* idt.c */

#include "core/idt.h"
#include "drivers/video/video.h"
#include "kernel.h"
#include "signal.h"
#include "stdio.h"

// PIC (Programmable Interrupt Controller) ports
#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI   0x20 // End-of-Interrupt command

// External declarations for ISR and IRQ handlers
extern void isr0 (void);
extern void isr1 (void);
extern void isr2 (void);
extern void isr3 (void);
extern void isr4 (void);
extern void isr5 (void);
extern void isr6 (void);
extern void isr7 (void);
extern void isr8 (void);
extern void isr9 (void);
extern void isr10 (void);
extern void isr11 (void);
extern void isr12 (void);
extern void isr13 (void);
extern void isr14 (void);
extern void isr15 (void);
extern void isr16 (void);
extern void isr17 (void);
extern void isr18 (void);
extern void isr19 (void);
extern void isr20 (void);
extern void isr21 (void);
extern void isr22 (void);
extern void isr23 (void);
extern void isr24 (void);
extern void isr25 (void);
extern void isr26 (void);
extern void isr27 (void);
extern void isr28 (void);
extern void isr29 (void);
extern void isr30 (void);
extern void isr31 (void);

extern void irq0 (void);
extern void irq1 (void);
extern void irq2 (void);
extern void irq3 (void);
extern void irq4 (void);
extern void irq5 (void);
extern void irq6 (void);
extern void irq7 (void);
extern void irq8 (void);
extern void irq9 (void);
extern void irq10 (void);
extern void irq11 (void);
extern void irq12 (void);
extern void irq13 (void);
extern void irq14 (void);
extern void irq15 (void);

extern void* isr_stub_table[];

// Array of C-level interrupt handlers
static irq_handler_t irq_handlers[16];

void register_irq_handler (int irq, irq_handler_t handler) {
        irq_handlers[irq] = handler;
}

// IDT entry structure
struct idt_entry {
        uint16_t base_lo;
        uint16_t sel;
        uint8_t  ist;
        uint8_t  flags;
        uint16_t base_mid;
        uint32_t base_hi;
        uint32_t reserved;
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
        uint16_t limit;
        uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr   idtp;

// Set up an IDT entry
static void
idt_set_gate (uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
        idt[num].base_lo  = (uint16_t) (base & 0xFFFF);
        idt[num].base_mid = (uint16_t) ((base >> 16) & 0xFFFF);
        idt[num].base_hi  = (uint32_t) ((base >> 32) & 0xFFFFFFFF);
        idt[num].sel      = sel;
        idt[num].ist      = 0;
        idt[num].flags    = flags;
        idt[num].reserved = 0;
}

// Default C-level handlers
void isr_handler (registers_t* regs) {
        // This is a very basic "safe" handler for unexpected exceptions.
        // It avoids calling panic() or other complex functions recursively,
        // which would cause a triple fault.
        (void) regs; // The registers are unused for now.

        // An unhandled exception occurred. For now, we just halt the system.
        // A more advanced kernel would print debug info, but that can be risky
        // if the video or console systems themselves are what crashed.
        __asm__ volatile("cli; hlt");
}

void irq_handler (registers_t* regs) {
        // If a custom handler is registered, call it.
        if (regs->int_no >= 32 && regs->int_no <= 47) {
                irq_handler_t handler = irq_handlers[regs->int_no - 32];
                if (handler) {
                        handler(regs);
                }
        }

        // Send End-of-Interrupt (EOI) to the PICs.
        if (regs->int_no >= 40) {
                outb(PIC2_CMD, PIC_EOI); // EOI to slave PIC
        }
        outb(PIC1_CMD, PIC_EOI); // EOI to master PIC
}

void idt_init (void) {
        // Set up the IDT pointer
        idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
        idtp.base  = (uint64_t) &idt;

        // Remap the PIC
        outb(PIC1_CMD, 0x11);
        outb(PIC2_CMD, 0x11);
        outb(PIC1_DATA, 0x20); // Master PIC vector offset
        outb(PIC2_DATA, 0x28); // Slave PIC vector offset
        outb(PIC1_DATA, 0x04); // Tell Master PIC there is a slave PIC at IRQ2
        outb(PIC2_DATA, 0x02); // Tell Slave PIC its cascade identity
        outb(PIC1_DATA, 0x01); // 8086/88 (MCS-80/85) mode
        outb(PIC2_DATA, 0x01);
        outb(PIC1_DATA, 0x0);
        outb(PIC2_DATA, 0x0);

        // Set up ISR entries
        idt_set_gate(0, (uint64_t) isr0, 0x08, 0x8E);
        idt_set_gate(1, (uint64_t) isr1, 0x08, 0x8E);
        idt_set_gate(2, (uint64_t) isr2, 0x08, 0x8E);
        idt_set_gate(3, (uint64_t) isr3, 0x08, 0x8E);
        idt_set_gate(4, (uint64_t) isr4, 0x08, 0x8E);
        idt_set_gate(5, (uint64_t) isr5, 0x08, 0x8E);
        idt_set_gate(6, (uint64_t) isr6, 0x08, 0x8E);
        idt_set_gate(7, (uint64_t) isr7, 0x08, 0x8E);
        idt_set_gate(8, (uint64_t) isr8, 0x08, 0x8E);
        idt_set_gate(9, (uint64_t) isr9, 0x08, 0x8E);
        idt_set_gate(10, (uint64_t) isr10, 0x08, 0x8E);
        idt_set_gate(11, (uint64_t) isr11, 0x08, 0x8E);
        idt_set_gate(12, (uint64_t) isr12, 0x08, 0x8E);
        idt_set_gate(13, (uint64_t) isr13, 0x08, 0x8E);
        idt_set_gate(14, (uint64_t) isr14, 0x08, 0x8E);
        idt_set_gate(15, (uint64_t) isr15, 0x08, 0x8E);
        idt_set_gate(16, (uint64_t) isr16, 0x08, 0x8E);
        idt_set_gate(17, (uint64_t) isr17, 0x08, 0x8E);
        idt_set_gate(18, (uint64_t) isr18, 0x08, 0x8E);
        idt_set_gate(19, (uint64_t) isr19, 0x08, 0x8E);
        idt_set_gate(20, (uint64_t) isr20, 0x08, 0x8E);
        idt_set_gate(21, (uint64_t) isr21, 0x08, 0x8E);
        idt_set_gate(22, (uint64_t) isr22, 0x08, 0x8E);
        idt_set_gate(23, (uint64_t) isr23, 0x08, 0x8E);
        idt_set_gate(24, (uint64_t) isr24, 0x08, 0x8E);
        idt_set_gate(25, (uint64_t) isr25, 0x08, 0x8E);
        idt_set_gate(26, (uint64_t) isr26, 0x08, 0x8E);
        idt_set_gate(27, (uint64_t) isr27, 0x08, 0x8E);
        idt_set_gate(28, (uint64_t) isr28, 0x08, 0x8E);
        idt_set_gate(29, (uint64_t) isr29, 0x08, 0x8E);
        idt_set_gate(30, (uint64_t) isr30, 0x08, 0x8E);
        idt_set_gate(31, (uint64_t) isr31, 0x08, 0x8E);

        // Set up IRQ entries
        idt_set_gate(32, (uint64_t) irq0, 0x08, 0x8E);
        idt_set_gate(33, (uint64_t) irq1, 0x08, 0x8E);
        idt_set_gate(34, (uint64_t) irq2, 0x08, 0x8E);
        idt_set_gate(35, (uint64_t) irq3, 0x08, 0x8E);
        idt_set_gate(36, (uint64_t) irq4, 0x08, 0x8E);
        idt_set_gate(37, (uint64_t) irq5, 0x08, 0x8E);
        idt_set_gate(38, (uint64_t) irq6, 0x08, 0x8E);
        idt_set_gate(39, (uint64_t) irq7, 0x08, 0x8E);
        idt_set_gate(40, (uint64_t) irq8, 0x08, 0x8E);
        idt_set_gate(41, (uint64_t) irq9, 0x08, 0x8E);
        idt_set_gate(42, (uint64_t) irq10, 0x08, 0x8E);
        idt_set_gate(43, (uint64_t) irq11, 0x08, 0x8E);
        idt_set_gate(44, (uint64_t) irq12, 0x08, 0x8E);
        idt_set_gate(45, (uint64_t) irq13, 0x08, 0x8E);
        idt_set_gate(46, (uint64_t) irq14, 0x08, 0x8E);
        idt_set_gate(47, (uint64_t) irq15, 0x08, 0x8E);

        // Load the IDT
        __asm__ volatile("lidt %0" : : "m"(idtp));
}
