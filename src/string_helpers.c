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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string_helpers.h"
#include "easprintf.h"

char* construct_func_strings(char* (*func)(void*, size_t, int*),
                             void* strings,
                             size_t strings_len,
                             size_t* ret_size)
{
    char* res_html, *result = NULL;
    int curr_parse_size = 0, last_parse_size, parse_size;

    if (strings_len <= 0) return NULL;

    for (size_t i = 0; i < strings_len; ++i)
    {
        res_html = func(strings, i, &parse_size);
        
        if (parse_size == -1) /* Malloc error */
        {
            if (result) free(result);
            return NULL;
        }
        last_parse_size = curr_parse_size;
        curr_parse_size += parse_size;
        
        result = realloc(result, curr_parse_size + 1);
        if (result == NULL)
        {
            perror("malloc");
            free(res_html);
            return NULL;
        }

        /* Copy res_html to result in correct position */
        strncpy(result + last_parse_size, res_html, parse_size);
        /* Cleanup */
        free(res_html);
    }
    
    result[curr_parse_size] = '\0';

    if (ret_size) *ret_size = curr_parse_size;

    return result;
}
