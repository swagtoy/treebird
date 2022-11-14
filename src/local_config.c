/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
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

