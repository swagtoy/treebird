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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "query.h"
#include "key.h"
#include "mime.h"

struct query_values post = { 0 };
struct get_values query = { 0 };

char* read_query_data(struct get_values* query)
{
    struct http_query_info info;
    char* query_string = getenv("QUERY_STRING");
    char* get_query = NULL, *g_query_read;

    // BEGIN Query references
    struct key_value_refs refs[] = {
        { "offset", &(query->offset), { 0 }, key_string },
        { "q", &(query->q), { 0 }, key_string },
    };
    // END Query references
    
    if (query_string)
    {
        get_query = malloc(strlen(query_string) + 1);
        if (!get_query)
        {
            perror("malloc");
            return NULL;
        }
        strcpy(get_query, query_string);
        
        // For shifting through
        g_query_read = get_query;

        do
        {
            g_query_read = parse_query(g_query_read, &info);
            if (!(info.key && info.val)) break;
            for (size_t i = 0; i < (sizeof(refs)/sizeof(refs[0])); ++i)
                if (strcmp(info.key, refs[i].key) == 0)
                    refs[i].func(info.val, NULL, info.key);
        }
        while (g_query_read);
    }

    return get_query;
}

char* read_post_data(struct query_values* post)
{
    struct http_query_info query_info;
    struct http_form_info form_info;
    char* request_method = getenv("REQUEST_METHOD");
    char* post_query = NULL, *p_query_read;

    // BEGIN Query references
    struct key_value_refs refs[] = {
        { "content", &(post->content), { 0 }, key_string },
        { "itype", &(post->itype), { 0 }, key_string },
        { "id", &(post->id), { 0 }, key_string },
        { "theme", &(post->theme), { 0 }, key_string },
        { "themeclr", &(post->themeclr), { 0 }, key_string },
        { "jsactions", &(post->jsactions), { 0 }, key_string },
        { "jsreply", &(post->jsreply), { 0 }, key_string },
        { "jslive", &(post->jslive), { 0 }, key_string },
        { "username", &(post->username), { 0 }, key_string },
        { "password", &(post->password), { 0 }, key_string },
        { "replyid", &(post->replyid), { 0 }, key_string },
        { "min_id", &(post->min_id), { 0 }, key_string },
        { "max_id", &(post->max_id), { 0 }, key_string },
        { "start_id", &(post->start_id), { 0 }, key_string }
    };
    // END Query references

    if (request_method && strcmp("POST", request_method) == 0)
    {
        char* mime_boundary;
        char* mime_mem = get_mime_boundary(NULL, &mime_boundary);
        int content_length = atoi(getenv("CONTENT_LENGTH"));
        post_query = malloc(content_length + 1);
        if (!post_query)
        {
            perror("malloc");
            return NULL;
        }

        // fread should be a macro to FCGI_fread, which is set by FCGI_Accept in previous definitions
        size_t len = fread(post_query, 1, content_length, stdin);
        post_query[content_length] = '\0';

        // For shifting through
        p_query_read = post_query;

        do
        {
            if (mime_mem)
            {
                // Mime value
                p_query_read = read_form_data(mime_boundary,
                                              p_query_read,
                                              &form_info);
                if (!p_query_read) break;
                for (size_t i = 0; i < (sizeof(refs)/sizeof(refs[0])); ++i)
                    if (strcmp(form_info.name, refs[i].key) == 0)
                        refs[i].func(form_info.value, &(refs[i].form), refs[i].val);
            }
            else {
                // Mime value not set
                p_query_read = parse_query(p_query_read, &query_info);
                if (!(query_info.key && query_info.val)) break;
                for (size_t i = 0; i < (sizeof(refs)/sizeof(refs[0])); ++i)
                    if (strcmp(query_info.key, refs[i].key) == 0)
                        refs[i].func(query_info.val, NULL, refs[i].val);
            }
        }
        while (p_query_read);

        if (mime_mem) free(mime_mem);
    }

    // Free me afterwards!
    return post_query;
}

char* parse_query(char* begin, struct http_query_info* info)
{
    int end = 0;
    char* val_begin;
    info->key = begin;
    for (; *begin != '&' && *begin != '\0'; ++begin)
    {
        if (*begin == '=')
        {
            val_begin = begin+1;
            *begin = '\0';
        }
    }

    if (*begin == '\0') end = 1;
    if (*begin == '&') *begin = '\0';
    info->val = val_begin;
        
    return end ? NULL : begin+1;
}

char* try_handle_post(void (*call)(struct http_query_info*, void*), void* arg)
{
    char* request_method = getenv("REQUEST_METHOD");
    char* post_query = NULL, * p_query_read;
    struct http_query_info info;
    
    // Handle POST
    if (request_method && (strcmp("POST", request_method) == 0))
    {
        int content_length = atoi(getenv("CONTENT_LENGTH"));
        post_query = malloc(content_length + 1);
        if (!post_query)
        {
            puts("Malloc error!");
            return NULL;
        }
        read(STDIN_FILENO, post_query, content_length);
        post_query[content_length] = '\0';
        

        // For parse_query to shift through, so we can still free the original
        p_query_read = post_query;

        // Parse
        while (1)
        {
            p_query_read = parse_query(p_query_read, &info);
            if (!(info.key && info.val)) break;
            call(&info, arg);

            if (!p_query_read) break;
        }
    }

    return post_query;
}
