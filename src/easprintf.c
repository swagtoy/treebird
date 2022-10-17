/*
 * Treebird - Lightweight frontend for Pleroma
 * Copyright (C) 2022 Nekobit
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
