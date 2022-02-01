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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../config.h"
#include "page_config.h"
#include "query.h"

// Pages
#include "../static/index.chtml"
#include "../static/config.chtml"

void content_config(mastodont_t* api)
{
    (void)api; // No need to use this
    char* request_method = getenv("REQUEST_METHOD");
    char* post_query, * p_query_read;
    struct http_query_info info;

    // Output
    printf("Content-Length: %ld\r\n\r\n",
           data_index_html_size + data_config_html_size);

    
    // Handle POST
    if (request_method && (strcmp("POST", request_method) == 0))
    {
        int content_length = atoi(getenv("CONTENT_LENGTH"));
        post_query = malloc(content_length + 1);
        if (!post_query)
        {
            puts("Malloc error!");
            return;
        }
        read(STDIN_FILENO, post_query, content_length);
        post_query[content_length] = '\0';

        // For parse_query to shift through, so we can still free the original
        p_query_read = post_query;

        // Parse
        while (1)
        {
            p_query_read = parse_query(p_query_read, &info);
            
            printf("%.*s,", (int)info.val_len, info.val);

            if (p_query_read == NULL)
                break;
        }

        // Cleanup
        free(post_query);
    }

    printf(data_index_html, config_canonical_name, data_config_html);
}
