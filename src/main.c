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

#include "global_perl.h"
#include <pthread.h>
#include <string.h>
#include "memory.h"
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
#include "memory_page.h"
#include "query.h"
#include "status.h"
#include "lists.h"
#include "timeline.h"
#include "session.h"
#include "notifications.h"
#include "env.h"
#include "search.h"
#include "about.h"
#include "local_config_set.h"
#include "global_cache.h"
#include "conversations.h"
#include "request.h"
#include "cgi.h"

// Allow dynamic loading for Perl
static void xs_init (pTHX);
void boot_DynaLoader (pTHX_ CV* cv);

#ifdef DEBUG
static int quit = 0;
static int exit_treebird(PATH_ARGS)
{
    quit = 1;
    exit(1);
}
#endif

/*******************
 *  Path handling  *
 ******************/
static struct path_info paths[] = {
    { "/config/general", content_config_general },
    { "/config/appearance", content_config_appearance },
    /* { "/config/account", content_config_account }, */
    { "/config", content_config },
    { "/login/oauth", content_login_oauth },
    { "/login", content_login },
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
    { "/notification/:/read", content_notifications_read },
    { "/notification/:/delete", content_notifications_clear },
    { "/notifications/read", content_notifications_read },
    { "/notifications/clear", content_notifications_clear },
    { "/notifications", content_notifications },
    { "/tag/:", content_tl_tag },
    { "/chats/:", content_chat_view },
    { "/chats", content_chats },
#ifdef DEBUG
    { "/quit", exit_treebird },
    { "/exit", exit_treebird },
#endif
    // Debug, but cool to see
    { "/memory_stats", content_memory_stats },
    // API
    { "/treebird_api/v1/notifications", api_notifications },
    { "/treebird_api/v1/interact", api_status_interact },
    { "/treebird_api/v1/attachment", api_attachment_create },
};

static int application(mastodont_t* api, REQUEST_T req)
{
    int rc;
    propagate_memory();

    // Default config
    struct session ssn = {
        .config = {
            .theme = "treebird20",
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
            .sidebar_opacity = 255,
        },
        .cookies = {{}},
        .post = {{}},
        .query = {{}},
        .acct = { 0 },
        .acct_storage = { 0 },
        .logged_in = 0,
    };
        
    // Load cookies
    char* cookies_str = read_cookies_env(req, &(ssn.cookies));
    char* post_str = read_post_data(req, &(ssn.post));
    char* get_str = read_get_data(req, &(ssn.query));

    // Read config options
    enum config_page page = CONFIG_GENERAL;
    char* path_info = GET_ENV("PATH_INFO", req);
    if (path_info && strcmp(path_info, "/config/appearance") == 0)
        page = CONFIG_APPEARANCE;
    struct mstdnt_storage* attachments = load_config(req, &ssn, api, page);

    // Load current account information
    get_account_info(api, &ssn);

    rc = handle_paths(req, &ssn, api, paths, sizeof(paths)/sizeof(paths[0]));

    // Cleanup
    if (cookies_str) tb_free(cookies_str);
    if (post_str) tb_free(post_str);
    if (get_str) tb_free(get_str);
    free_files(&(keyfile(ssn.post.files)));
    if (ssn.logged_in) mstdnt_cleanup_account(&(ssn.acct));
    mstdnt_storage_cleanup(&(ssn.acct_storage));
    if (attachments)
        cleanup_media_storages(&ssn, attachments);
    
    return rc;
}

#ifndef CGI_MODE
static void fcgi_start(mastodont_t* api)
{
    int rc;
    FCGX_Request req;

    while (1)
    {
        FCGX_InitRequest(&req, 0, 0);

        struct mstdnt_fd fds[] = {
            {
                // The docs says not to use this directly, but we don't care
                // what the docs say
                .fd = req.listen_sock,
                .events = MSTDNT_POLLIN,
            }
        };
        
        mstdnt_poll(api, 0, fds, sizeof(fds)/sizeof(fds[0]));
        
        rc = FCGX_Accept_r(&req);
        if (rc < 0) break;

        rc = application(api, &req);

        if (rc != 1)
            FCGX_Finish_r(&req);
    }
}
#else
void cgi_start(mastodont_t* api)
{
    while (FCGI_Accept() >= 0
#ifdef DEBUG
           && quit == 0)
#else
        )
#endif
    {
        application(api, NULL);
    }
}
#endif

void xs_init(pTHX)
{
    static const char file[] = __FILE__;
    dXSUB_SYS;
    PERL_UNUSED_CONTEXT;
    
    newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
}

int main(int argc, char **argv, char **env)
{
    // Global init
    mstdnt_global_curl_init();
#ifndef CGI_MODE
    FCGX_Init();
#endif

    // Initialize Perl
    PERL_SYS_INIT3(&argc, &argv, &env);
    my_perl = perl_alloc();
    perl_construct(my_perl);
    //char* perl_argv[] = { "", "-e", data_main_pl, NULL };
    char* perl_argv[] = { "", "-I", "perl/", "perl/main.pl", NULL };

    perl_parse(my_perl, xs_init, (sizeof(perl_argv) / sizeof(perl_argv[0])) - 1, perl_argv, (char**)NULL);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    PL_perl_destruct_level = 1;
    perl_run(my_perl);

    init_template_files(aTHX);

    struct mstdnt_hooks hooks = {
        .malloc = tb_malloc,
        .free = tb_free,
        .calloc = tb_calloc,
        .realloc = tb_realloc,
    };
    mstdnt_set_hooks(&hooks);

    // Initiate mstdnt library
    mastodont_t api;
    mstdnt_init(&api);
    // Fetch information about the current instance
//    load_instance_info_cache(&api);

#ifndef CGI_MODE
    // Hell, let's not sit around here either
    fcgi_start(&api);
    // Blocks
    
    FCGX_ShutdownPending();
#else
    cgi_start(&api);
#endif
   
//    free_instance_info_cache();
    mstdnt_global_curl_cleanup();
    mstdnt_cleanup(&api);

    cleanup_template_files();

    perl_destruct(my_perl);
    perl_free(my_perl);
    PERL_SYS_TERM();
    return 0;
}
