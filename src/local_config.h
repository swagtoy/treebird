/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef LOCAL_CONFIG_H
#define LOCAL_CONFIG_H
#include "global_perl.h"
#include "query.h"

struct local_config
{
    char* logged_in;
    char* theme;
    char* background_url;
    int lang;
    int jsactions;
    int jsreply;
    int jslive;
    int js;
    int interact_img;
    int stat_attachments;
    int stat_greentexts;
    int stat_dope;
    int stat_oneclicksoftware;
    int stat_emojo_likes;
    int stat_hide_muted;
    int instance_show_shoutbox;
    int instance_panel;
    int notif_embed;
    int sidebar_opacity;
};

HV* perlify_config(struct local_config* config);

#endif // LOCAL_CONFIG_H
