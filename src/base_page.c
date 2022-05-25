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
#include "../static/index.chtml"
#include "../static/quick_login.chtml"

#define BODY_STYLE "style=\"background:url('%s');\""

void render_base_page(struct base_page* page, struct session* ssn, mastodont_t* api)
{
    char* cookie = getenv("HTTP_COOKIE");
    enum l10n_locale locale = page->locale;
    const char* login_string = "<a href=\"login\" id=\"login-header\">Login / Register</a>";
    const char* sidebar_embed = "<iframe class=\"sidebar-frame\" src=\"/notifications_compact\"></iframe>";
    char* background_url_css = NULL;
    // Sidebar
    char* sidebar_str,
        * main_sidebar_str = NULL,
        * account_sidebar_str = NULL;
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
        easprintf(&main_sidebar_str, data_quick_login_html,
                  config_url_prefix,
                  L10N[L10N_EN_US][L10N_USERNAME],
                  L10N[L10N_EN_US][L10N_PASSWORD],
                  L10N[L10N_EN_US][L10N_LOGIN_BTN]);
    }

    // Combine into sidebar
    easprintf(&sidebar_str, "%s%s",
              account_sidebar_str ? account_sidebar_str : "",
              main_sidebar_str ? main_sidebar_str : "");

    char* data;
    int len = easprintf(&data, data_index_html,
                        L10N[locale][L10N_APP_NAME],
                        ssn->config.theme,
                        ssn->config.themeclr ? "-dark" : "",
                        background_url_css ? background_url_css : "",
                        config_url_prefix,
                        L10N[locale][L10N_APP_NAME],
                        login_string,
                        config_url_prefix,
                        L10N[locale][L10N_SEARCH_PLACEHOLDER],
                        L10N[locale][L10N_SEARCH_BUTTON],
                        CAT_TEXT(page->category, BASE_CAT_HOME),
                        config_url_prefix,
                        L10N[locale][L10N_HOME],
                        CAT_TEXT(page->category, BASE_CAT_LOCAL),
                        config_url_prefix,
                        L10N[locale][L10N_LOCAL],
                        CAT_TEXT(page->category, BASE_CAT_FEDERATED),
                        config_url_prefix,
                        L10N[locale][L10N_FEDERATED],
                        CAT_TEXT(page->category, BASE_CAT_NOTIFICATIONS),
                        config_url_prefix,
                        L10N[locale][L10N_NOTIFICATIONS],
                        CAT_TEXT(page->category, BASE_CAT_LISTS),
                        config_url_prefix,
                        L10N[locale][L10N_LISTS],
                        CAT_TEXT(page->category, BASE_CAT_FAVOURITES),
                        config_url_prefix,
                        L10N[locale][L10N_FAVOURITES],
                        CAT_TEXT(page->category, BASE_CAT_BOOKMARKS),
                        config_url_prefix,
                        L10N[locale][L10N_BOOKMARKS],
                        CAT_TEXT(page->category, BASE_CAT_DIRECT),
                        config_url_prefix,
                        L10N[locale][L10N_DIRECT],
                        CAT_TEXT(page->category, BASE_CAT_CONFIG),
                        config_url_prefix,
                        L10N[locale][L10N_CONFIG],
                        page->sidebar_left ? page->sidebar_left : "",
                        (ssn->config.instance_panel && g_cache.panel_html.response ?
                         g_cache.panel_html.response : ""),
                        page->content,
                        sidebar_str ? sidebar_str : "");
    
    if (!data)
    {
        perror("malloc");
        goto cleanup;
    }
    
    render_html(data, len);

    // Cleanup
/* cleanup_all: */
    free(data);
cleanup:
    free(sidebar_str);
    if (main_sidebar_str != sidebar_embed) free(main_sidebar_str);
    free(account_sidebar_str);
    free(background_url_css);
}

void render_html(char* data, size_t data_len)
{
    fputs("Content-type: text/html\r\n", stdout);
    printf("Content-Length: %d\r\n\r\n", data_len + 1);
    puts(data);    
}
