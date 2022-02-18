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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "query.h"

char* parse_query(char* begin, struct http_query_info* info)
{
    int end = 0;
    char* val_begin;
    info->key = begin;
    for (; *begin != '&' && *begin != '\0'; ++begin)
    {
        if (*begin == '=')
        {
            val_begin = begin+1;
            *begin = '\0';
        }
    }

    if (*begin == '\0') end = 1;
    if (*begin == '&') *begin = '\0';
    info->val = val_begin;
    // The val length may be large, so strlen can waste time
    info->val_len = (size_t)(begin - val_begin);
        
    return end ? NULL : begin+1;
}

char* try_handle_post(void (*call)(struct http_query_info*, void*), void* arg)
{
    char* request_method = getenv("REQUEST_METHOD");
    char* post_query = NULL, * p_query_read;
    struct http_query_info info;
    
    // Handle POST
    if (request_method && (strcmp("POST", request_method) == 0))
    {
        int content_length = atoi(getenv("CONTENT_LENGTH"));
        post_query = malloc(content_length + 1);
        if (!post_query)
        {
            puts("Malloc error!");
            return NULL;
        }
        read(STDIN_FILENO, post_query, content_length);
        post_query[content_length] = '\0';
        

        // For parse_query to shift through, so we can still free the original
        p_query_read = post_query;

        // Parse
        while (1)
        {
            p_query_read = parse_query(p_query_read, &info);
            if (!(info.key && info.val)) break;
            call(&info, arg);

            if (!p_query_read) break;
        }
    }

    return post_query;
}
