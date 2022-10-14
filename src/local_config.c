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
#include "local_config.h"

HV* perlify_config(struct local_config* config)
{
    HV* ssn_config_hv = newHV();

    hv_stores(ssn_config_hv, "logged_in", newSVpv(config->logged_in, 0));
    hv_stores(ssn_config_hv, "theme", newSVpv(config->theme, 0));
    hv_stores(ssn_config_hv, "background_url", newSVpv(config->background_url, 0));
    hv_stores(ssn_config_hv, "lang", newSViv(config->lang));
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

