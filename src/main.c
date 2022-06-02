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
#include "search.h"
#include "about.h"
#include "local_config_set.h"
#include "global_cache.h"

int main(void)
{
    unsigned run_count = 1;

    // Global init
    mastodont_global_curl_init();

    // Initiate mastodont library
    mastodont_t api;
    mastodont_init(&api, MSTDNT_FLAG_NO_URI_SANITIZE | config_library_flags);
    api.url = config_instance_url;

    // Fetch information about the current instance
    load_instance_info_cache(&api);

    /*******************
     *  Path handling  *
     ******************/
    struct path_info paths[] = {
        { "/config/general", content_config_general },
        { "/config/appearance", content_config_appearance },
        /* { "/config/account", content_config_account }, */
        { "/config", content_config },
        { "/login/oauth", content_login_oauth },
        { "/login", content_login },
        { "/test", content_test },
        { "/user/:/action/:", content_account_action },
        { "/@:/scrobbles", content_account_scrobbles },
        { "/@:/pinned", content_account_pinned },
        { "/@:/media", content_account_media },
        { "/@:", content_account_statuses },
        { "/status/:/react/:", content_status_react },
        { "/status/:/react", status_emoji },
        { "/status/create", content_status_create },
        { "/status/:/interact", status_interact },
        { "/status/:/reply", status_reply },
        { "/status/:", status_view },
        { "/notice/:", notice_redirect },
        { "/about/license", content_about_license },
        { "/about", content_about },
        { "/search/statuses", content_search_statuses },
        { "/search/accounts", content_search_accounts },
        { "/search/hashtags", content_search_hashtags },
        { "/search", content_search_statuses },
        { "/lists/for/:", content_tl_list },
        { "/lists", content_lists },
        { "/federated", content_tl_federated },
        { "/direct", content_tl_direct },
        { "/local", content_tl_local },
        { "/bookmarks", content_account_bookmarks },
        { "/favourites", content_account_favourites },
        { "/notifications_compact", content_notifications_compact },
        { "/notifications", content_notifications },
        { "/tag/:", content_tl_tag },
        // API
        { "/treebird_api/v1/notifications", api_notifications },
        { "/treebird_api/v1/interact", api_status_interact }
    };

    while (FCGI_Accept() >= 0)
    {
        // Default config
        struct session ssn = {
            .config = {
                .theme = "treebird20",
                .themeclr = 0,
                .jsactions = 1,
                .jsreply = 1,
                .jslive = 0,
                .js = 1,
                .stat_attachments = 1,
                .stat_greentexts = 1,
                .stat_dope = 1,
                .stat_oneclicksoftware = 1,
                .stat_emojo_likes = 0,
                .stat_hide_muted = 0,
                .instance_show_shoutbox = 1,
                .instance_panel = 1,
                .notif_embed = 1,
            },
            .cookies = {{}},
            .post = {{}},
            .query = {{}},
            .acct = { 0 },
            .acct_storage = { 0 },
            .logged_in = 0,
        };
        
        // Load cookies
        char* cookies_str = read_cookies_env(&(ssn.cookies));
        char* post_str = read_post_data(&(ssn.post));
        char* get_str = read_get_data(&(ssn.query));

        // Instance info temp stuff
        if (keystr(ssn.cookies.instance_url))
            api.url = keystr(ssn.cookies.instance_url);
        else
            api.url = config_instance_url;
        
        api.token = keystr(ssn.cookies.access_token); // Load token now

        // Read config options
        load_config(&ssn, &api);

        // Load current account information
        get_account_info(&api, &ssn);

        handle_paths(&ssn, &api, paths, sizeof(paths)/sizeof(paths[0]));

        // Cleanup
        if (cookies_str) free(cookies_str);
        if (post_str) free(post_str);
        if (get_str) free(get_str);
        free_files(&(keyfile(ssn.post.files)));
        if (ssn.logged_in) mstdnt_cleanup_account(&(ssn.acct));
        mastodont_storage_cleanup(&(ssn.acct_storage));

        ++run_count;
    }

    free_instance_info_cache();
    mastodont_cleanup(&api);
    mastodont_global_curl_cleanup();
}
