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
#include "base_page.h"
#include "../config.h"
#include "page_config.h"
#include "query.h"
#include "cookie.h"
#include "local_config.h"

// Pages
#include "../static/index.chtml"
#include "../static/config.chtml"

void content_config(mastodont_t* api, char** data, size_t size)
{
    (void)api; // No need to use this
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
            if (!(info.key && info.val)) break;
            if (strcmp(info.key, "theme") == 0)
            {
                g_config.theme = info.val;
                printf("Set-Cookie: %s=%s; HttpOnly; SameSite=Strict;",
                       "theme", info.val);
                g_config.changed = 1;
            }

            if (!p_query_read) break;
        }

    }

    struct base_page b = {
        .locale = L10N_EN_US,
        .content = data_config_html,
        .sidebar_right = NULL
    };

    render_base_page(&b);
    
    // Cleanup
    if (post_query) free(post_query);
}
