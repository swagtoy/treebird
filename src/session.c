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

#include "session.h"
#include "../config.h"

const char* const get_instance(struct session* ssn)
{
    return keystr(ssn->cookies.instance_url) ?
        keystr(ssn->cookies.instance_url) : config_instance_url;
}

const char* const get_token(struct session* ssn)
{
    return keystr(ssn->cookies.access_token);
}

static HV* perlify_post_values(struct post_values* post)
{
}

static HV* perlify_get_values(struct get_values* get)
{
}

static HV* perlify_config(struct local_config* config)
{
    HV* ssn_config_hv = newHV();

    hv_stores(ssn_config_hv, "logged_in", newSVpv(config->logged_in, 0));
    hv_stores(ssn_config_hv, "theme", newSVpv(config->theme, 0));
    hv_stores(ssn_config_hv, "background_url", newSVpv(config->background_url, 0));
    hv_stores(ssn_config_hv, "lang", newSViv(config->lang));
    hv_stores(ssn_config_hv, "themeclr", newSViv(config->themeclr));
    hv_stores(ssn_config_hv, "jsactions", newSViv(config->jsactions));
    hv_stores(ssn_config_hv, "jslive", newSViv(config->jslive));
    hv_stores(ssn_config_hv, "js", newSViv(config->js));
    hv_stores(ssn_config_hv, "interact_img", newSViv(config->interact_img));
    hv_stores(ssn_config_hv, "stat_attachments", newSViv(config->stat_attachments));
    hv_stores(ssn_config_hv, "stat_greentexts", newSViv(config->stat_greentexts));
    hv_stores(ssn_config_hv, "stat_dope", newSViv(config->stat_dope));
    hv_stores(ssn_config_hv, "stat_oneclicksoftware", newSViv(config->stat_oneclicksoftware));
    hv_stores(ssn_config_hv, "stat_emojo_likes", newSViv(config->stat_emojo_likes));
    hv_stores(ssn_config_hv, "stat_hide_muted", newSViv(config->stat_hide_muted));
    hv_stores(ssn_config_hv, "instance_show_shoutbox", newSViv(config->instance_show_shoutbox));
    hv_stores(ssn_config_hv, "instance_panel", newSViv(config->instance_panel));
    hv_stores(ssn_config_hv, "notif_embed", newSViv(config->notif_embed));
    hv_stores(ssn_config_hv, "sidebar_opacity", newSViv(config->sidebar_opacity));
    
    return ssn_config_hv;
}

HV* perlify_session(struct session* ssn)
{
    HV* ssn_hv = newHV();
    hv_stores(ssn_hv, "logged_in", newSVuv(ssn->logged_in));
//    HV* ssn_post_values = perlify_post_values(&(ssn->post));
//    HV* ssn_get_values = perlify_get_values(&(ssn->query));
//    HV* ssn_get_values = perlify_cookie_values(&(ssn->query))
    // Config
    HV* ssn_config = perlify_config(&(ssn->config));
    hv_stores(ssn_hv, "config", newRV_inc((SV*)ssn_config));
    

    return ssn_hv;
}

