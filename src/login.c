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
#include <stdio.h>
#include "base_page.h"
#include "login.h"
#include "../config.h"

// Files
#include "../static/login.chtml"

void content_login(mastodont_t* api, char** data, size_t data_size)
{
    struct mstdnt_storage storage;
    struct mstdnt_app app;
    struct mstdnt_app_register_args args = {
        .client_name = "RatFE",
        .redirect_uris = "http://localhost/",
        .scopes = "read+write",
        .website = NULL
    };
    mastodont_register_app(api, &args, &storage, &app);
    
    struct base_page b = {
        .locale = L10N_EN_US,
        .content = app.client_id,
        .sidebar_right = NULL
    };

    // Output
    render_base_page(&b);
}
