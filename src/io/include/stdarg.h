#pragma once

/*
 * stdarg.h - Variable arguments handling (C99 standard)
 * This header provides macros for functions with variable argument lists.
 * Supports GCC/Clang builtins on supported architectures (e.g., x86_64).
 */

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 * Architecture-specific va_list definition
 * For compilers supporting built-in va_list (GCC, Clang)
 *----------------------------------------------------------------------------*/
#if defined(__GNUC__)

/* va_list type built-in to the compiler */
typedef __builtin_va_list va_list;

/* Initialize ap to point to first unnamed argument after last named parameter
 */
#define va_start(ap, last) __builtin_va_start((ap), (last))

/* Retrieve next argument of given type */
#define va_arg(ap, type) __builtin_va_arg((ap), type)

/* Clean up the va_list when done */
#define va_end(ap) __builtin_va_end(ap)

/* Copy state of one va_list to another */
#ifndef va_copy
#define va_copy(dest, src) __builtin_va_copy((dest), (src))
#endif

#else
#error "stdarg.h requires GCC or Clang built-in va_list support"
#endif

#ifdef __cplusplus
}
#endif