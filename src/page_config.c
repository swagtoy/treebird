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
#include <string.h>
#include "http.h"
#include "base_page.h"
#include "../config.h"
#include "easprintf.h"
#include "page_config.h"
#include "query.h"
#include "cookie.h"
#include "local_config_set.h"
#include "string_helpers.h"
#include "l10n.h"

// Pages
#include "../static/config_general.chtml"
#include "../static/config_appearance.chtml"
#include "../static/config_sidebar.chtml"

#define bool_checked(key) (ssn->config.key ? "checked" : "")

enum config_category
{
    CONFIG_CAT_GENERAL,
    CONFIG_CAT_APPEARANCE,
    CONFIG_CAT_ACCOUNT
};

static char* construct_config_sidebar(enum config_category cat, size_t* size)
{
    char* sidebar_html;
    size_t s = easprintf(&sidebar_html, data_config_sidebar_html,
                         CAT_TEXT(cat, CONFIG_CAT_GENERAL),
                         config_url_prefix,
                         L10N[L10N_EN_US][L10N_GENERAL],
                         CAT_TEXT(cat, CONFIG_CAT_APPEARANCE),
                         config_url_prefix,
                         L10N[L10N_EN_US][L10N_APPEARANCE],
                         CAT_TEXT(cat, CONFIG_CAT_ACCOUNT),
                         config_url_prefix,
                         L10N[L10N_EN_US][L10N_ACCOUNT]);
    if (size) *size = s;
    return sidebar_html;
}

void content_config_general(struct session* ssn, mastodont_t* api, char** data)
{
    char* sidebar_html = construct_config_sidebar(CONFIG_CAT_GENERAL, NULL);
    
    char* general_page;

    easprintf(&general_page, data_config_general_html,
              bool_checked(js),
              bool_checked(jsactions),
              bool_checked(jsreply),
              bool_checked(jslive),
              bool_checked(stat_attachments),
              bool_checked(stat_greentexts),
              bool_checked(stat_dope),
              bool_checked(stat_oneclicksoftware),
              bool_checked(stat_emojo_likes),
              bool_checked(stat_hide_muted),
              bool_checked(instance_show_shoutbox),
              bool_checked(instance_panel),
              bool_checked(notif_embed));

    load_config(ssn, api);
    
    struct base_page b = {
        .category = BASE_CAT_CONFIG,
        .locale = L10N_EN_US,
        .content = general_page,
        .sidebar_left = sidebar_html
    };

    render_base_page(&b, ssn, api);
    // Cleanup
    free(sidebar_html);
    free(general_page);
}


void content_config_appearance(struct session* ssn, mastodont_t* api, char** data)
{
    char* sidebar_html = construct_config_sidebar(CONFIG_CAT_APPEARANCE, NULL);

    load_config(ssn, api);

    struct base_page b = {
        .category = BASE_CAT_CONFIG,
        .locale = L10N_EN_US,
        .content = data_config_appearance_html,
        .sidebar_left = sidebar_html
    };

    render_base_page(&b, ssn, api);
    // Cleanup
    free(sidebar_html);
}

void content_config(struct session* ssn, mastodont_t* api, char** data)
{
    redirect(REDIRECT_303, "/config/general");
}
