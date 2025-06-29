#pragma once

// stdio implementations
#include "print/print.h"
#include "putchar/putchar.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"
#include "strlen/strlen.h"

size_t strlen (const char* str);
void*  memcpy (void* dest, const void* src, size_t n);
void   putchar (int c);
void   puthex (uint64_t n);
void   putdec (uint32_t n);