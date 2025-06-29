#pragma once

#include "stdint.h"

// This struct defines the registers that our ISR/IRQ stubs push to the stack.
// We receive a pointer to this in our C-level interrupt handlers.
typedef struct {
        uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
        uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
        uint64_t int_no, err_code;
} registers_t;