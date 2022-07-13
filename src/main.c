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
#include "emoji.h"
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
#include "conversations.h"

int main(void)
{
    unsigned run_count = 1;

    // Global init
    mastodont_global_curl_init();

    // Initiate mastodont library
    mastodont_t api;
    mastodont_init(&api);

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
        { "/user/:", content_account_statuses },
        { "/@:/scrobbles", content_account_scrobbles },
        { "/@:/pinned", content_account_pinned },
        { "/@:/media", content_account_media },
        { "/@:/following", content_account_following },
        { "/@:/followers", content_account_followers },
        { "/@:/statuses", content_account_statuses },
        { "/@:", content_account_statuses },
        { "/status/:/react/:", content_status_react },
        { "/status/:/react", status_emoji },
        { "/status/create", content_status_create },
        { "/status/:/interact", status_interact },
        { "/status/:/reply", status_reply },
        { "/status/:/favourited_by", status_view_favourites },
        { "/status/:/boosted_by", status_view_reblogs },
        { "/status/:/reblogged_by", status_view_reblogs },
        { "/status/:", status_view },
        { "/notice/:", notice_redirect },
        { "/about/license", content_about_license },
        { "/about", content_about },
        { "/search/statuses", content_search_statuses },
        { "/search/accounts", content_search_accounts },
        { "/search/hashtags", content_search_hashtags },
        { "/search", content_search_all },
        { "/emoji_picker", content_emoji_picker },
        { "/lists/edit/:", list_edit },
        { "/lists/for/:", content_tl_list },
        { "/lists", content_lists },
        { "/local", content_tl_local },
        { "/federated", content_tl_federated },
        { "/direct", content_tl_direct },
        { "/bookmarks", content_account_bookmarks },
        { "/favourites", content_account_favourites },
        { "/blocked", content_account_blocked },
        { "/muted", content_account_muted },
        { "/notifications_compact", content_notifications_compact },
        { "/notifications", content_notifications },
        { "/tag/:", content_tl_tag },
        { "/chats/:", content_chat_view },
        { "/chats", content_chats },
        { "/chats_embed/:", content_chat_embed },
        // API
        { "/treebird_api/v1/notifications", api_notifications },
        { "/treebird_api/v1/interact", api_status_interact },
        { "/treebird_api/v1/attachment", api_attachment_create },
    };

    while (FCGI_Accept() >= 0)
    {
        // Default config
        struct session ssn = {
            .config = {
                .theme = "treebird20",
                .themeclr = 0,
                .lang = L10N_EN_US,
                .jsactions = 1,
                .jsreply = 1,
                .jslive = 0,
                .js = 1,
                .interact_img = 0,
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

        // Read config options
        enum config_page page = CONFIG_GENERAL;
        char* path_info = getenv("PATH_INFO");
        if (path_info && strcmp(path_info, "/config/appearance") == 0)
            page = CONFIG_APPEARANCE;
        load_config(&ssn, &api, page);

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
