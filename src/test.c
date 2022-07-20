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
#include "../static/test.ctmpl"

#define ENV_NOT_FOUND "<span style=\"color:red;\">ENV Not Found</span>"

enum env_tbl_index
{
    ENV_HTTP_COOKIE = 0,
    ENV_PATH_INFO,
    ENV_QUERY_STRING,
    ENV_REQUEST_METHOD,
    ENV_SCRIPT_NAME,
    ENV_HTTP_REFERER,
    ENV_HTTP_USER_AGENT,
    ENV_CONTENT_LENGTH,
};

#define ENV_TBL_GET(index) (env_tbl[(index)] ? env_tbl[(index)] : ENV_NOT_FOUND)

void content_test(PATH_ARGS)
{
    char* env_tbl[] = {
        GET_ENV("HTTP_COOKIE", req),
        GET_ENV("PATH_INFO", req),
        GET_ENV("QUERY_STRING", req),
        GET_ENV("REQUEST_METHOD", req),
        GET_ENV("SCRIPT_NAME", req),
        GET_ENV("HTTP_REFERER", req),
        GET_ENV("HTTP_USER_AGENT", req),
        GET_ENV("CONTENT_LENGTH", req)
    };
    
    char* page;
    struct test_template tdata = {
        .HTTP_COOKIE = ENV_TBL_GET(ENV_HTTP_COOKIE),
        .PATH_INFO = ENV_TBL_GET(ENV_PATH_INFO),
        .QUERY_STRING = ENV_TBL_GET(ENV_QUERY_STRING),
        .REQUEST_METHOD = ENV_TBL_GET(ENV_REQUEST_METHOD),
        .SCRIPT_NAME = ENV_TBL_GET(ENV_SCRIPT_NAME),
        .HTTP_REFERER = ENV_TBL_GET(ENV_HTTP_REFERER),
        .HTTP_USER_AGENT = ENV_TBL_GET(ENV_HTTP_USER_AGENT),
        .CONTENT_LENGTH = ENV_TBL_GET(ENV_CONTENT_LENGTH)
    };
    page = tmpl_gen_test(&tdata, NULL);
    
    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = page,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, req, ssn, api);
    if (page) free(page);
}
