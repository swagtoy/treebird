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

#include <stdlib.h>
#include <string.h>
#include "test.h"
#include "../config.h"
#include "base_page.h"
#include "easprintf.h"

// Pages
#include "../static/test.chtml"

#define ENV_NOT_FOUND "<span style=\"color:red;\">ENV Not Found</span>"

enum env_tbl_index
{
    ENV_HTTP_COOKIES = 0,
    ENV_PATH_INFO,
    ENV_QUERY_STRING,
    ENV_REQUEST_METHOD,
    ENV_SCRIPT_NAME,
    ENV_HTTP_REFERER,
    ENV_HTTP_USER_AGENT,
    ENV_CONTENT_LENGTH,
};

#define ENV_TBL_GET(index) (env_tbl[(index)] ? env_tbl[(index)] : ENV_NOT_FOUND)

void content_test(mastodont_t* api, char** data, size_t data_size)
{
    char* env_tbl[] = {
        getenv("HTTP_COOKIES"),
        getenv("PATH_INFO"),
        getenv("QUERY_STRING"),
        getenv("REQUEST_METHOD"),
        getenv("SCRIPT_NAME"),
        getenv("HTTP_REFERER"),
        getenv("HTTP_USER_AGENT"),
        getenv("CONTENT_LENGTH")
    };
    
    char* page;
    easprintf(&page,
              data_test_html,
              ENV_TBL_GET(ENV_HTTP_COOKIES),
              ENV_TBL_GET(ENV_PATH_INFO),
              ENV_TBL_GET(ENV_QUERY_STRING),
              ENV_TBL_GET(ENV_REQUEST_METHOD),
              ENV_TBL_GET(ENV_SCRIPT_NAME),
              ENV_TBL_GET(ENV_HTTP_REFERER),
              ENV_TBL_GET(ENV_HTTP_USER_AGENT),
              ENV_TBL_GET(ENV_CONTENT_LENGTH));
    
    struct base_page b = {
        .locale = L10N_EN_US,
        .content = page,
        .sidebar_right = NULL
    };

    // Output
    render_base_page(&b, api);
    if (page) free(page);
}
