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
#include "base_page.h"
#include "about.h"

#include "../static/about.ctmpl"
#include "../static/license.ctmpl"

void content_about(PATH_ARGS)
{
    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = (char*)data_about,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, req, ssn, api);
}


void content_about_license(PATH_ARGS)
{
    char* page;
    char* referer = GET_ENV("HTTP_REFERER", req);
    struct license_template tdata = {
        .back_ref = referer,
        .license_str = "License"
    };
    page = tmpl_gen_license(&tdata, NULL);
    
    struct base_page b = {
        .category = BASE_CAT_NONE,
        .content = page,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, req, ssn, api);
    free(page);
}

