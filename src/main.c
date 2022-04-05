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
#include <mastodont.h>
#include <stdlib.h>
#include "../config.h"
#include "index.h"
#include "page_config.h"
#include "path.h"
#include "account.h"
#include "login.h"
#include "local_config.h"
#include "cookie.h"
#include "query.h"
#include "status.h"
#include "lists.h"
#include "timeline.h"
#include "session.h"
#include "notifications.h"
#include "test.h"

int main(void)
{
    // Global init
    mastodont_global_curl_init();

    unsigned run_count = 1;

    // API
    while (FCGI_Accept() >= 0)
    {
        struct session ssn = {
            .config = {
                .changed = 0,
                .theme = "treebird20",
                .themeclr = 0,
                .jsactions = 0,
                .jsreply = 0,
                .jslive = 0
            },
            .cookies = { 0 },
            .post = { 0 },
            .query = { 0 }
        };
        
        // Load cookies
        char* cookies_str = read_cookies_env(&(ssn.cookies));
        char* post_str = read_post_data(&(ssn.post));
        char* get_str = read_query_data(&(ssn.query));

        mastodont_t api;
        if (ssn.cookies.instance_url)
            api.url = ssn.cookies.instance_url;
        else
            api.url = config_instance_url;
        mastodont_init(&api, MSTDNT_FLAG_NO_URI_SANITIZE | config_library_flags);
        api.token = ssn.cookies.access_token; // Load token now

        /*******************
         *  Path handling  *
         ******************/
        struct path_info paths[] = {
            { "/config/general", content_config_general },
            { "/config/appearance", content_config_appearance },
            { "/config", content_config },
            /* { "/config/account", content_config_account }, */
            { "/login", content_login },
            { "/test", content_test },
            { "/@:/action/:", content_account_action },
            { "/@:", content_account },
            { "/status/create", content_status_create },
            { "/status/:/interact", status_interact },
            { "/status/:/reply", status_reply },
            { "/status/:", status_view },
            { "/lists/for/:", content_tl_list },
            { "/lists", content_lists },
            { "/federated", content_tl_federated },
            { "/local", content_tl_local },
            { "/notifications", content_notifications },
        };

        handle_paths(&ssn, &api, paths, sizeof(paths)/sizeof(paths[0]));

        // Cleanup
        if (cookies_str) free(cookies_str);
        if (post_str) free(post_str);
        if (get_str) free(get_str);
        mastodont_free(&api);

        ++run_count;
    }

    mastodont_global_curl_cleanup();
}
