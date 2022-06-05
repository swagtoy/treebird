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
#include "base_page.h"
#include "easprintf.h"
#include "cookie.h"
#include "notifications.h"
#include "string_helpers.h"
#include "../config.h"
#include "local_config_set.h"
#include "account.h"
#include "global_cache.h"

// Files
#include "../static/index.ctmpl"
#include "../static/quick_login.ctmpl"

#define BODY_STYLE "style=\"background:url('%s');\""

void render_base_page(struct base_page* page, struct session* ssn, mastodont_t* api)
{
    char* cookie = getenv("HTTP_COOKIE");
    enum l10n_locale locale = l10n_normalize(ssn->config.lang);
    const char* login_string = "<a href=\"login\" id=\"login-header\">Login / Register</a>";
    const char* sidebar_embed = "<iframe class=\"sidebar-frame\" src=\"/notifications_compact\"></iframe>";
    char* background_url_css = NULL;
    // Sidebar
    char* sidebar_str,
        * main_sidebar_str = NULL,
        * account_sidebar_str = NULL,
        * instance_str = NULL;
    // Mastodont, used for notifications sidebar
    struct mstdnt_storage storage = { 0 };
    struct mstdnt_notification* notifs = NULL;
    size_t notifs_len = 0;

    if (ssn->config.logged_in)
        login_string = "";

    if (ssn->config.background_url)
    {
        easprintf(&background_url_css, BODY_STYLE, ssn->config.background_url);
    }

    // If user is logged in
    if (keystr(ssn->cookies.logged_in) && keystr(ssn->cookies.access_token))
    {
        account_sidebar_str = construct_account_sidebar(&(ssn->acct), NULL);

        // Get / Show notifications on sidebar
        if (ssn->config.notif_embed)
        {
            main_sidebar_str = (char*)sidebar_embed;
        }
        else {
            struct mstdnt_get_notifications_args args = {
                .exclude_types = 0,
                .account_id = NULL,
                .exclude_visibilities = 0,
                .include_types = 0,
                .with_muted = 1,
                .max_id = NULL,
                .min_id = NULL,
                .since_id = NULL,
                .offset = 0,
                .limit = 8,
            };
        
            if (mastodont_get_notifications(api, &args, &storage, &notifs, &notifs_len) == 0)
            {
                main_sidebar_str = construct_notifications_compact(ssn, api, notifs, notifs_len, NULL);
            }

            mstdnt_cleanup_notifications(notifs, notifs_len);
            mastodont_storage_cleanup(&storage);
        }
    }
    else {
        // Construct small login page
        struct quick_login_template tdata = {
            .prefix = config_url_prefix,
            .username = L10N[locale][L10N_USERNAME],
            .password = L10N[locale][L10N_PASSWORD],
            .login = L10N[locale][L10N_LOGIN_BTN],
        };
        main_sidebar_str = tmpl_gen_quick_login(&tdata, NULL);
    }

    // Combine into sidebar
    easprintf(&sidebar_str, "%s%s",
              account_sidebar_str ? account_sidebar_str : "",
              main_sidebar_str ? main_sidebar_str : "");

    // Create instance panel
    easprintf(&instance_str, "<div class=\"static-html\" id=\"instance-panel\">%s</div>",
              (g_cache.panel_html.response ?
               g_cache.panel_html.response : ""));

    struct index_template index = {
        .title = L10N[locale][L10N_APP_NAME],
        .theme = ssn->config.theme,
        .theme_clr = ssn->config.themeclr ? "-dark" : "",
        .prefix = config_url_prefix,
        .background_url = background_url_css,
        .name = L10N[locale][L10N_APP_NAME],
        .sidebar_cnt = login_string,
        .placeholder = L10N[locale][L10N_SEARCH_PLACEHOLDER],
        .search_btn = L10N[locale][L10N_SEARCH_BUTTON],
        .active_home = CAT_TEXT(page->category, BASE_CAT_HOME),
        .home = L10N[locale][L10N_HOME],
        .active_local = CAT_TEXT(page->category, BASE_CAT_LOCAL),
        .local = L10N[locale][L10N_LOCAL],
        .active_federated = CAT_TEXT(page->category, BASE_CAT_FEDERATED),
        .federated = L10N[locale][L10N_FEDERATED],
        .active_notifications = CAT_TEXT(page->category, BASE_CAT_NOTIFICATIONS),
        .notifications = L10N[locale][L10N_NOTIFICATIONS],
        .active_lists = CAT_TEXT(page->category, BASE_CAT_LISTS),
        .lists = L10N[locale][L10N_LISTS],
        .active_favourites = CAT_TEXT(page->category, BASE_CAT_FAVOURITES),
        .favourites = L10N[locale][L10N_FAVOURITES],
        .active_bookmarks = CAT_TEXT(page->category, BASE_CAT_BOOKMARKS),
        .bookmarks = L10N[locale][L10N_BOOKMARKS],
        .active_direct = CAT_TEXT(page->category, BASE_CAT_DIRECT),
        .direct = L10N[locale][L10N_DIRECT],
        .active_config = CAT_TEXT(page->category, BASE_CAT_CONFIG),
        .config = L10N[locale][L10N_CONFIG],
        .sidebar_leftbar = page->sidebar_left,
        .instance_panel = ssn->config.instance_panel ? instance_str : "",
        .main = page->content,
        .sidebar_rightbar = sidebar_str
    };
    
    size_t len;
    char* data = tmpl_gen_index(&index, &len);
    
    if (!data)
    {
        perror("malloc");
        goto cleanup;
    }
    
    send_result(NULL, "text/html", data, len);

    // Cleanup
/* cleanup_all: */
    free(data);
cleanup:
    free(sidebar_str);
    if (main_sidebar_str != sidebar_embed) free(main_sidebar_str);
    free(account_sidebar_str);
    free(background_url_css);
    free(instance_str);
}

void send_result(char* status, char* content_type, char* data, size_t data_len)
{
    if (data_len == 0) data_len = strlen(data);
    printf("Status: %s\r\n"
           "Content-type: %s\r\n"
           "Content-Length: %d\r\n\r\n",
           status ? status : "200 OK",
           content_type ? content_type : "text/html",
           data_len + 1);
    puts(data);    
}
