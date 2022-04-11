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

struct file_content
{
    char* content;
    size_t content_size;
    char* filetype;
    char* filename;
};

struct file_array
{
    struct file_content* content;
    size_t array_size;
};

struct query_values
{
    // Config
    char* theme;
    char* themeclr;
    char* jsactions;
    char* jsreply;
    char* jslive;
    
    char* content;
    char* itype;
    char* id;
    char* username;
    char* password;
    char* replyid;
    char* min_id;
    char* max_id;
    char* start_id;

    struct file_array files;
};

struct get_values
{
    char* offset;
    char* q;
};

void key_files(char* val, struct form_props* form, void* arg);

char* read_query_data(struct get_values* query);
char* read_post_data(struct query_values* post);
/* A stupidly quick query parser */
char* parse_query(char* begin, struct http_query_info* info);
char* try_handle_post(void (*call)(struct http_query_info*, void*), void* arg);

void free_files(struct file_array* files);

#endif // QUERY_H
