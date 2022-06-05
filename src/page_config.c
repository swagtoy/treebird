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
#include "../static/config_general.ctmpl"
#include "../static/config_appearance.ctmpl"
#include "../static/config_sidebar.ctmpl"

#define bool_checked(key) (ssn->config.key ? "checked" : "")

enum config_category
{
    CONFIG_CAT_GENERAL,
    CONFIG_CAT_APPEARANCE,
    CONFIG_CAT_ACCOUNT
};

static char* construct_config_sidebar(enum config_category cat, size_t* size)
{
    struct config_sidebar_template tdata = {
        .prefix = config_url_prefix,
        .general_active = CAT_TEXT(cat, CONFIG_CAT_GENERAL),
        .general = L10N[L10N_EN_US][L10N_GENERAL],
        .appearance_active = CAT_TEXT(cat, CONFIG_CAT_APPEARANCE),
        .appearance = L10N[L10N_EN_US][L10N_APPEARANCE],
        .account_active = CAT_TEXT(cat, CONFIG_CAT_ACCOUNT),
        .account = L10N[L10N_EN_US][L10N_ACCOUNT],
    };
    
    return tmpl_gen_config_sidebar(&tdata, size);
}

void content_config_general(struct session* ssn, mastodont_t* api, char** data)
{
    char* sidebar_html = construct_config_sidebar(CONFIG_CAT_GENERAL, NULL);

    struct config_general_template tdata = {
        .js_on = bool_checked(js),
        .jsactions_on = bool_checked(jsactions),
        .jsreply_on = bool_checked(jsreply),
        .jslive_on = bool_checked(jslive),
        .status_attachments_on = bool_checked(stat_attachments),
        .status_greentexts_on = bool_checked(stat_greentexts),
        .status_dopameme_on = bool_checked(stat_dope),
        .status_oneclicksoftware_on = bool_checked(stat_oneclicksoftware),
        .status_emojo_likes_on = bool_checked(stat_emojo_likes),
        .status_hide_muted_on = bool_checked(stat_hide_muted),
        .instance_show_shoutbox_on = bool_checked(instance_show_shoutbox),
        .instance_panel_on = bool_checked(instance_panel),
        .notifications_embed_on = bool_checked(notif_embed)
    };

    char* general_page = tmpl_gen_config_general(&tdata, NULL);
    
    struct base_page b = {
        .category = BASE_CAT_CONFIG,
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

    struct base_page b = {
        .category = BASE_CAT_CONFIG,
        .content = data_config_appearance,
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
