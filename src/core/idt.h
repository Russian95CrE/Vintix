#pragma once

#include "registers.h"

void idt_init (void);

typedef void (*irq_handler_t)(registers_t*);
void register_irq_handler (int irq, irq_handler_t handler);