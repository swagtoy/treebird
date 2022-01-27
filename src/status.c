/*
 * RatFE - Lightweight frontend for Pleroma
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

#include <stdlib.h>
#include <string.h>
#include "status.h"
#include "easprintf.h"
#include "../static/status.chtml"

char* construct_statuses(struct mstdnt_status* statuses, size_t size, size_t* ret_size)
{
    char* stat_html, *result = NULL;
    int curr_parse_size = 0, last_parse_size, parse_size;

    for (size_t i = 0; i < size; ++i)
    {
        parse_size = easprintf(&stat_html, data_status_html,
                               statuses[i].id, /* Username */
                               "@who?", /* Account */
                               "Public?", /* visibility */
                               statuses[i].content);
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
            free(stat_html);
            return NULL;
        }

        /* Copy stat_html to result in correct position */
        strncpy(result + last_parse_size, stat_html, parse_size);
        /* Cleanup */
        free(stat_html);
    }
    
    result[curr_parse_size] = '\0';

    if (ret_size) *ret_size = curr_parse_size;

    return result;
}

