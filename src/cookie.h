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

struct cookie_values
{
    char* access_token;
    char* logged_in;
    char* theme;
    char* instance_url;
    char* background_url;
    char* client_id;
    char* client_secret;
    int themeclr;
    int jsactions;
    int jsreply;
    int jslive;
    int js;
    int stat_attachments;
    int stat_greentexts;
    int stat_dope;
    int stat_oneclicksoftware;
    int stat_emoji_likes;
    int instance_show_shoutbox;
    int instance_panel;
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
