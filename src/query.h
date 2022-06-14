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

#ifndef QUERY_H
#define QUERY_H
#include <fcgi_stdio.h>
#include <stddef.h>
#include "key.h"

struct http_query_info
{
    char* key;
    char* val;
};

struct post_values
{
    // Config
    struct key theme; // String
    struct key themeclr; // Int
    struct key lang; // Int
    struct key title; // String
    struct key jsactions; // Int
    struct key jsreply; // Int
    struct key jslive; // Int
    struct key js; // Int
    struct key stat_attachments; // Int
    struct key stat_greentexts; // Int
    struct key stat_dope; // Int
    struct key stat_oneclicksoftware; // Int
    struct key stat_emojo_likes; // Int
    struct key stat_hide_muted; // Int
    struct key instance_show_shoutbox; // Int
    struct key instance_panel; // Int
    struct key notif_embed; // Int
    struct key set; // Int
    struct key only_media; // Int
    struct key replies_only; // Int
    
    struct key content; // String
    struct key itype; // String
    struct key id; // String
    struct key username; // String
    struct key password; // String
    struct key replyid; // String
    struct key visibility; // String
    struct key instance; // String
    struct key emojoindex; // Int

    // Navigation
    struct key min_id; // String
    struct key max_id; // String
    struct key start_id; // String

    struct key files; // Files
};

struct get_values
{
    struct key offset; // String
    struct key query; // String
    struct key code; // String
};

char* read_get_data(struct get_values* query);
char* read_post_data(struct post_values* post);
/* A stupidly quick query parser */
char* parse_query(char* begin, struct http_query_info* info);
char* try_handle_post(void (*call)(struct http_query_info*, void*), void* arg);

void free_files(struct file_array* files);

#endif // QUERY_H
