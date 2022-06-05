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

#ifndef COOKIE_H
#define COOKIE_H
#include <stddef.h>
#include "key.h"

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
    struct key stat_attachments;
    struct key stat_greentexts;
    struct key stat_dope;
    struct key stat_oneclicksoftware;
    struct key stat_emojo_likes;
    struct key stat_hide_muted;
    struct key instance_show_shoutbox;
    struct key instance_panel;
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
char* read_cookies_env(struct cookie_values* cookies);
int cookie_get_val(char* src, char* key, struct http_cookie_info* info);

#endif // COOKIE_H
