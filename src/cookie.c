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
#include "cookie.h"

enum cookie_state
{
    STATE_C_NEUTRAL,
    STATE_C_STARTSP,
    STATE_K_START,
    STATE_V_START,
};

char* read_cookies_env(struct cookie_values* cookies)
{
    struct http_cookie_info info;
    char* cookies_env = getenv("HTTP_COOKIE");

    // Is it even work bothering with?
    if (!cookies_env)
        return NULL;
    
    char* cookies_str = malloc(strlen(cookies_env) + 1);
    if (!cookies_str)
    {
        perror("malloc");
        return NULL;
    }
    strcpy(cookies_str, cookies_env);
    char* cookies_read = cookies_str;

    // Will loop through these
    struct key_value_refs refs[] = {
        { "access_token", &(cookies->access_token), key_string },
        { "logged_in", &(cookies->logged_in), key_string },
        { "theme", &(cookies->theme), key_string },
        { "lang", &(cookies->lang), key_int },
        { "instance_url", &(cookies->instance_url), key_string },
        { "background_url", &(cookies->background_url), key_string },
        { "interact_img", &(cookies->interact_img), key_int },
        { "client_id", &(cookies->client_id), key_string },
        { "client_secret", &(cookies->client_secret), key_string },
        { "themeclr", &(cookies->themeclr), key_int },
        { "jsactions", &(cookies->jsactions), key_int },
        { "notifembed", &(cookies->notif_embed), key_int },
        { "jsreply", &(cookies->jsreply), key_int },
        { "jslive", &(cookies->jslive), key_int },
        { "js", &(cookies->js), key_int },
        { "statattachments", &(cookies->stat_attachments), key_int },
        { "statgreentexts", &(cookies->stat_greentexts), key_int },
        { "statdope", &(cookies->stat_dope), key_int },
        { "statoneclicksoftware", &(cookies->stat_oneclicksoftware), key_int },
        { "statemojolikes", &(cookies->stat_emojo_likes), key_int },
        { "stathidemuted", &(cookies->stat_hide_muted), key_int },
        { "instanceshowshoutbox", &(cookies->instance_show_shoutbox), key_int },
        { "instancepanel", &(cookies->instance_panel), key_int },
    };

    do
    {
        cookies_read = parse_cookies(cookies_read, &info);

        if (!(info.key && info.val)) break;
        for (int i = 0; i < (sizeof(refs)/sizeof(refs[0])); ++i)
        {
            if (strcmp(info.key, refs[i].key) == 0)
            {
                refs[i].func(info.val, NULL, refs[i].val);
                refs[i].val->is_set = 1;
            }
        }
    }
    while (cookies_read);

    // User is responsible for freeing when done!
    return cookies_str;
}

char* parse_cookies(char* begin, struct http_cookie_info* info)
{
    int keydone = 0;
    enum cookie_state state = STATE_C_STARTSP;
    int end = 0;
    size_t val_s = 0;
    
    for (; *begin != ';' && *begin != '\0'; ++begin)
    {
        switch (*begin)
        {
        case '=':
            if (state == STATE_K_START) keydone = 1;
            state = STATE_C_STARTSP;
            *begin = '\0'; // Null ptr
            break;
        case ' ':
            // longkeyval = res;
            //           ^ ^
            if (state == STATE_C_STARTSP ||
                state == STATE_K_START)
                break;
            // fall
        default:
            if (state == STATE_C_STARTSP)
            {
                if (keydone) info->val = begin;
                else info->key = begin;
                state = keydone ? STATE_V_START : STATE_K_START;
            }
            if (keydone) ++val_s;
        }
    }

    // Which character did we stop at?
    if (*begin == '\0')
        end = 1;
    else if (*begin == ';')
        *begin = '\0';
    
    // The val length may be large, so strlen can waste time
    info->val_len = val_s;
    
    return end ? NULL : begin+1;
}

int cookie_get_val(char* src, char* key, struct http_cookie_info* info)
{
    struct http_cookie_info read_info;
    char* src_read = src;
    
    while (1)
    {
        src_read = parse_cookies(src_read, &read_info);

        if (!(read_info.key && read_info.val)) break;
        if (strcmp(read_info.key, key) == 0)
        {
            info->key = read_info.key;
            info->val = read_info.val;
            info->val_len = read_info.val_len;
            return 0;
        }

        if (!src_read) break;
    }

    return 1;
}
