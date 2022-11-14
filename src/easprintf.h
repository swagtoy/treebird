/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef EASPRINTF_H
#define EASPRINTF_H
#include <stdarg.h>

/* Alternatives to asprintf functions, which are glibc-only/bsd functions */
int evasprintf(char** ret, const char* format, va_list ap);
int easprintf(char** ret, const char* format, ...);

#endif // EASPRINTF
