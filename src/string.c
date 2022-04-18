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

#include <stddef.h>
#include <string.h>
#include "string.h"

int streql(char* cmp1, char* cmp2)
{
    while (*cmp1 || *cmp2)
        if (*cmp1++ != *cmp2++)
            return 0;
    
    return 1;
}


int strneql(char* cmp1, char* cmp2, size_t cmp1_n)
{
    for (size_t i = 0; i < cmp1_n; ++i)
        if (*cmp1++ != *cmp2++)
            return 0;
    
    return 1;
}


char* strnstr(const char* haystack, const char* needle, size_t s)
{
    size_t needle_len = strlen(needle);
    for (size_t i = 0; i < s; ++i)
    {
        if (strneql(needle, haystack + i, needle_len))
            return haystack + i;
    }

    return NULL;
}

char* strrepl(char* source, char* find, char* repl)
{
    size_t repl_len = strlen(repl);
    char* result = NULL;
    char* n;
    char* needle = source;

    do
    {
        n = strstr(find);
        if (!n) break;
        result = realloc(result, n - source + repl_len + 1);
        result[n - source + repl_len] = '\0';
        // Copy initial string up to here
        strncpy(result, find, n - source - 1);
        
    } while (n);

    // Return source string if no replacements
    return result ? result : source;
}
