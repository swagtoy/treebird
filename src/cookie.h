/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef COOKIE_H
#define COOKIE_H
#include <stddef.h>
#include "global_perl.h"
#include "key.h"
#include "cgi.h"
#include "request.h"

struct cookie_values
{
    struct key access_token;
    struct key logged_in;
    struct key theme;
    struct key lang;
    struct key instance_url;
    struct key background_url;
    struct key client_id;
    struct key client_secret;
    struct key themeclr;
    struct key jsactions;
    struct key jsreply;
    struct key jslive;
    struct key js;
    struct key interact_img;
    struct key stat_attachments;
    struct key stat_greentexts;
    struct key stat_dope;
    struct key stat_oneclicksoftware;
    struct key stat_emojo_likes;
    struct key stat_hide_muted;
    struct key instance_show_shoutbox;
    struct key instance_panel;
    struct key sidebar_opacity;
    struct key notif_embed;
};

struct http_cookie_info
{
    char* key;
    char* val;
    size_t val_len; // Val may be large, like CSS property
};

// Stupidly fast simple cookie parser
char* parse_cookies(char* begin, struct http_cookie_info* info);
char* read_cookies_env(REQUEST_T req, struct cookie_values* cookies);
int cookie_get_val(char* src, char* key, struct http_cookie_info* info);

HV* perlify_cookies(struct cookie_values* cookies);

#endif // COOKIE_H
