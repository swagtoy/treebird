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

void content_config(mastodont_t* api, char** data, size_t size)
{
    (void)api; // No need to use this

    if (post.theme)
    {
        g_config.theme = post.theme;
        printf("Set-Cookie: %s=%s; HttpOnly; SameSite=Strict;",
               "theme", post.theme);
        g_config.changed = 1;
    }

    struct base_page b = {
        .locale = L10N_EN_US,
        .content = data_config_html,
        .sidebar_right = NULL
    };

    render_base_page(&b, api);
}
