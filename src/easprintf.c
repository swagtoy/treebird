/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#include <fcgi_stdio.h>
#include <stdlib.h>
#include "global_perl.h"
#include "easprintf.h"

int evasprintf(char** ret, const char* format, va_list ap)
{
    va_list ap_copy;
    va_copy(ap_copy, ap);

    int sz = vsnprintf(NULL, 0, format, ap);
    va_end(ap);

    *ret = tb_malloc(sz + 1);
    if(*ret == NULL)
    {
        perror("malloc");
        *ret = NULL;
        va_end(ap_copy);
        return -1;
    }

    vsnprintf(*ret, sz + 1, format, ap_copy);

    va_end(ap_copy);
    return sz;
}

int easprintf(char** ret, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    return evasprintf(ret, format, ap);
}
