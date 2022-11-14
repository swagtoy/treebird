/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#include <fcgi_stdio.h>
#include "global_perl.h"
#include <string.h>
#include <stdlib.h>
#include "string_helpers.h"
#include "easprintf.h"

char* construct_func_strings(char* (*func)(void*, size_t, size_t*),
                             void* strings,
                             size_t strings_len,
                             size_t* ret_size)
{
    char* res_html, *result = NULL;
    size_t curr_parse_size = 0, last_parse_size, parse_size;

    if (strings_len <= 0) return NULL;

    for (size_t i = 0; i < strings_len; ++i)
    {
        res_html = func(strings, i, &parse_size);

        // Don't bother if it returns null
        // Null is equivalent to ""
        if (!res_html) continue;
        
        if (parse_size == -1) /* Malloc error */
        {
            if (result) tb_free(result);
            return NULL;
        }
        last_parse_size = curr_parse_size;
        curr_parse_size += parse_size;
        
        result = tb_realloc(result, curr_parse_size + 1);
        if (result == NULL)
        {
            perror("malloc");
            tb_free(res_html);
            return NULL;
        }

        // Copy res_html to result in correct position
        strncpy(result + last_parse_size, res_html, parse_size);
        // Cleanup
        tb_free(res_html);
    }

    if (result)
        result[curr_parse_size] = '\0';

    if (ret_size) *ret_size = curr_parse_size;

    return result;
}
