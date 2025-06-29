#pragma once

#include "stdarg.h"
#include "stddef.h"
#include "stdint.h"

void puthex (uint64_t n);
void putdec (uint32_t n);
void puts (const char* s);
void duts (const char* s);
int  vsnprintf (char* buffer, size_t size, const char* format, va_list args);
int  printf (const char* format, ...);
int  debugf (const char* format, ...);
void putchar (int c);