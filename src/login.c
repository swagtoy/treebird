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
#include "query.h"
#include "base_page.h"
#include "login.h"
#include "../config.h"

// Files
#include "../static/login.chtml"

struct login_info
{
    char* username;
    char* password;
};

static void authenticate(struct http_query_info* info, void* _args)
{
    struct login_info* login = _args;

    if (strcmp(info->key, "username") == 0)
        login->username = info->val;
    else if (strcmp(info->key, "password") == 0)
        login->password = info->val;
}

void content_login(mastodont_t* api, char** data, size_t data_size)
{
    char* post_query;
    struct mstdnt_storage storage, oauth_store;
    struct mstdnt_app app;
    struct mstdnt_oauth_token token;
    struct login_info info = { 0 };

    post_query = try_handle_post(authenticate, &info);
    if (post_query)
    {
        // Getting the client id/secret
        struct mstdnt_app_register_args args_app = {
            .client_name = "RatFE",
            .redirect_uris = "http://localhost/",
            .scopes = "read+write+follow+push",
            .website = NULL
        };

        mastodont_register_app(api, &args_app, &storage, &app);
    
        struct mstdnt_oauth_token_args args_token = {
            .grant_type = "password",
            .client_id = app.client_id,
            .client_secret = app.client_secret,
            .username = info.username,
            .password = info.password
        };
    
        mastodont_obtain_oauth_token(api, &args_token, &oauth_store,
                                     &token);
        // TODO checking, also ^ returns non-zero
        printf("Set-Cookie: access_token=%s; HttpOnly; SameSite=Strict;\r\n", token.access_token);
        printf("Set-Cookie: logged_in=t; SameSite=Strict");
//        cookie_get_val(cookies, key, http_cookie_info* &info)
    }

    struct base_page b = {
        .locale = L10N_EN_US,
        .content = data_login_html,
        .sidebar_right = NULL
    };

    // Output
    render_base_page(&b);

    // Cleanup
    if (post_query) free(post_query);
}
