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
                   int value)
{
    if (ssn)
    {
        *ssn = value;
        printf("Set-Cookie: %s=%ld; HttpOnly; Path=/; Max-Age=31536000; SameSite=Strict;\r\n",
               cookie_name, value);
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
    set_config_str(&(ssn->config.theme), "theme", ssn->post.theme, ssn->cookie.theme);
    set_config_int(&(ssn->config.themeclr), "themeclr", ssn->post.themeclr, ssn->cookie.themeclr);

    set_config_int(&(ssn->config.jsactions), "jsactions", ssn->cookie.theme, ssn->post.jsactions);
    set_config_int(&(ssn->config.jsreply), "jsreply", ssn->cookie.theme, ssn->post.jsreply);
    set_config_int(&(ssn->config.jslive), "jslive", ssn->cookie.theme, ssn->post.jslive);
    set_config_int(&(ssn->config.js), "js", ssn->cookie.theme, ssn->post.js);
    set_config_int(&(ssn->config.stat_attachments), "statattachments", ssn->cookie.theme, ssn->post.stat_attachments);
    set_config_int(&(ssn->config.stat_greentexts), "statgreentexts", ssn->cookie.theme, ssn->post.stat_greentexts);
    set_config_int(&(ssn->config.stat_dope), "statdope", ssn->cookie.theme, ssn->post.stat_dope);
    set_config_int(&(ssn->config.stat_oneclicksoftware), "statoneclicksoftware", ssn->cookie.theme, ssn->post.stat_oneclicksoftware);
    set_config_int(&(ssn->config.stat_emoji_likes), "statemojilikes", ssn->cookie.theme, ssn->post.stat_emoji_likes);
    set_config_int(&(ssn->config.instance_show_shoutbox), "instanceshowshoutbox", ssn->cookie.theme, ssn->post.instance_show_shoutbox);
    set_config_int(&(ssn->config.instance_panel), "instancepanel", ssn->cookie.theme, ssn->post.instance_panel);
}
