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

static void config_post(struct http_query_info* info, void* args)
{
    (void)args;
    
    if (strcmp(info->key, "theme") == 0)
    {
        g_config.theme = info->val;
        printf("Set-Cookie: %s=%s; HttpOnly; SameSite=Strict;",
               "theme", info->val);
        g_config.changed = 1;
    }
}

void content_config(mastodont_t* api, char** data, size_t size)
{
    char* post_query;
    (void)api; // No need to use this
    
    post_query = try_handle_post(config_post, NULL);

    struct base_page b = {
        .locale = L10N_EN_US,
        .content = data_config_html,
        .sidebar_right = NULL
    };

    render_base_page(&b);
    
    // Cleanup
    if (post_query) free(post_query);
}
