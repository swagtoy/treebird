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
#include <stdlib.h>
#include "local_config_set.h"

int set_config_str(char** ssn,
                   char* cookie_name,
                   char* value)
{
    if (value && ssn)
    {
        *ssn = value;
        printf("Set-Cookie: %s=%s; HttpOnly; Path=/; SameSite=Strict;\r\n",
               cookie_name, value);
    }

    // If ssn isn't passed but value is set, then that means
    // something has changed
    return value != NULL;
}

int set_config_int(int* ssn,
                   char* cookie_name,
                   char* value)
{
    if (!value) return 0;
    char* err = NULL;
    long str_l = strtol(value, &err, 0);

    // Not a number
    if (err == value)
        return 0;

    if (ssn)
    {
        *ssn = str_l;
        printf("Set-Cookie: %s=%ld; HttpOnly; Path=/; Max-Age=31536000; SameSite=Strict;\r\n",
               cookie_name, str_l);
    }
    
    return 1;
}

void load_config(struct session* ssn, mastodont_t* api)
{
    if (ssn->post.theme)
    {
        struct mstdnt_attachment* attachments = NULL;
        struct mstdnt_storage* storage = NULL;
        if (try_upload_media(&storage, ssn, api, &attachments, NULL) == 0)
        {
            set_config_str(&(ssn->config.background_url), "background_url", attachments[0].url);
        }

        if (storage)
            cleanup_media_storages(ssn, storage);
    }
    set_config_str(&(ssn->config.theme), "theme", ssn->post.theme);
    set_config_int(&(ssn->config.themeclr), "themeclr", ssn->post.themeclr);

    set_config_int(&(ssn->config.jsactions), "jsactions", ssn->post.jsactions);
    set_config_int(&(ssn->config.jsreply), "jsreply", ssn->post.jsreply);
    set_config_int(&(ssn->config.jslive), "jslive", ssn->post.jslive);
}

#define SET_COOKIE_CONFIG(post, cookie, config) do{ \
        if (cookie && !post)                        \
            config = cookie;                        \
    } while(0); 

void read_config(struct session* ssn)
{
    SET_COOKIE_CONFIG(ssn->post.theme,
                      ssn->cookies.theme,
                      ssn->config.theme);
    SET_COOKIE_CONFIG(ssn->post.themeclr,
                      ssn->cookies.themeclr,
                      ssn->config.themeclr);
    SET_COOKIE_CONFIG(ssn->post.jsactions,
                      ssn->cookies.jsactions,
                      ssn->config.jsactions);
    SET_COOKIE_CONFIG(ssn->post.jsreply,
                      ssn->cookies.jsreply,
                      ssn->config.jsreply);
    SET_COOKIE_CONFIG(ssn->post.jslive,
                      ssn->cookies.jslive,
                      ssn->config.jslive);
    SET_COOKIE_CONFIG(0,
                      ssn->cookies.logged_in,
                      ssn->config.logged_in);
    SET_COOKIE_CONFIG((ssn->post.files.content && ssn->post.files.content[0].content),
                      ssn->cookies.background_url,
                      ssn->config.background_url);
}
