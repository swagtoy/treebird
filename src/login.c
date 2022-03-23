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

#include <fcgi_stdio.h>
#include <string.h>
#include <stdlib.h>
#include "query.h"
#include "base_page.h"
#include "login.h"
#include "error.h"
#include "easprintf.h"
#include "../config.h"

// Files
#include "../static/login.chtml"

void content_login(mastodont_t* api, char** data, size_t data_size)
{
    struct mstdnt_storage storage = { 0 }, oauth_store = { 0 };
    struct mstdnt_app app;
    struct mstdnt_oauth_token token;
    char* error = NULL;
    char* page;

    printf("%s: %s\r\n", post.username ? post.username:  "none", post.password ? post.password : "none");
    if (post.username && post.password)
    {
        // Getting the client id/secret
        struct mstdnt_args args_app = {
            .client_name = "treebird",
            .redirect_uris = "http://localhost/",
            .scopes = "read+write+follow+push",
            .website = NULL
        };

        mastodont_register_app(api, &args_app, &storage, &app);
    
        struct mstdnt_args args_token = {
            .grant_type = "password",
            .client_id = app.client_id,
            .client_secret = app.client_secret,
            .redirect_uri = NULL,
            .scope = NULL,
            .code = NULL,
            .username = post.username,
            .password = post.password
        };

        if (mastodont_obtain_oauth_token(api, &args_token, &oauth_store,
                                         &token) == 1)
        {
            error = construct_error(oauth_store.error, NULL);
        }
        else {
            // TODO checking, also ^ returns non-zero
            fputs("Status: 302 Found\r\n", stdout);
            printf("Set-Cookie: access_token=%s; Path=/; Max-Age=31536000\r\n", token.access_token);
            printf("Set-Cookie: logged_in=t; Path=/; Max-Age=31536000\r\n");
            // if config_url_prefix is empty, make it root
            printf("Location: %s\r\n\r\nRedirecting...",
                   config_url_prefix[0] == '\0' ?
                   "/" : config_url_prefix);
            return;
        }
    }

    // Concat
    easprintf(&page, data_login_html,
              L10N[L10N_EN_US][L10N_LOGIN],
              error ? error : "",
              config_url_prefix,
              L10N[L10N_EN_US][L10N_USERNAME],
              L10N[L10N_EN_US][L10N_PASSWORD],
              L10N[L10N_EN_US][L10N_LOGIN_BTN]);
    
    struct base_page b = {
        .locale = L10N_EN_US,
        .content = page,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, api);

    // Cleanup
    mastodont_storage_cleanup(&storage);
    mastodont_storage_cleanup(&oauth_store);
    if (error) free(error);
    if (page) free(page);
}
