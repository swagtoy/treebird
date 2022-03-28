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

struct http_query_info
{
    char* key;
    char* val;
};

struct query_values
{
    char* theme;
    char* content;
    char* itype;
    char* id;
    char* username;
    char* password;
    char* replyid;
};

struct get_values
{
    char* offset;
    char* id;
};

extern struct query_values post;
extern struct get_values query;

char* read_query_data();
char* read_post_data();
/* A stupidly quick query parser */
char* parse_query(char* begin, struct http_query_info* info);
char* try_handle_post(void (*call)(struct http_query_info*, void*), void* arg);

#endif // QUERY_H
