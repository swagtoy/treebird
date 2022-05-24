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

#define _XOPEN_SOURCE
#define _DEFAULT_SOURCE
#include <time.h>
#include <math.h>
#include "easprintf.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "string.h"

char* reltime_to_str(time_t stime)
{
    char* str;
    // Get current time and convert it to GMT
    time_t curr_time = time(NULL);
    int since = curr_time - stime;

    // Timezone likely off
    if (since < 60)
        easprintf(&str, "%ds", since);
    else if (since < 60 * 60)
        easprintf(&str, "%dm", since / 60);
    else if (since < 60 * 60 * 24)
        easprintf(&str, "%dh", since / (60 * 60));
    else if (since < 60 * 60 * 24 * 31) // Not truly but werks
        easprintf(&str, "%dd", since / (60 * 60 * 24));
    else if (since < 60 * 60 * 24 * 365)
        easprintf(&str, "%dmon", since / (60 * 60 * 24 * 31));
    else
        easprintf(&str, "%dyr", since / (60 * 60 * 24 * 365));
    
    return str;
}

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

char* strrepl(char* source, char* find, char* repl, int flags)
{
    const size_t find_len = strlen(find);
    const size_t repl_len = strlen(repl);
    char* result = NULL;
    char* curr;
    size_t is_last = 0;
    char* last = source;
    size_t str_size = 0;
    size_t last_str_size;

    do
    {
        if (*last == '\0') break;
        curr = strstr(last, find);
        if (last == source && !curr) break;
        // Move to end
        else if (!curr)
        {
            curr = last + strlen(last);
            is_last = 1;
        }
        
        // Increase to distance
        last_str_size = str_size;
        str_size += curr - last;

        // Create and copy
        result = realloc(result, str_size + (!is_last ? repl_len : 0) + 1);
        strncpy(result + last_str_size, last, curr - last);
        if (!is_last)
        {
            strncpy(result + str_size, repl, repl_len);

            // Bump past replace size and null term
            str_size += repl_len;
        }
        result[str_size] = '\0';
        // If is_last is true, this value doesn't matter
        last = curr + find_len;
    } while (flags == STRREPL_ALL && curr && !is_last);

    // Return source string if no replacements
    return result ? result : source;
}

