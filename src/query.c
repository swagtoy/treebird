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

#include "query.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "env.h"
#include "mime.h"
#include "cgi.h"


char* read_get_data(REQUEST_T req, struct get_values* query)
{
    struct http_query_info info = { 0 };
    char* query_string = GET_ENV("QUERY_STRING", req);
    char* get_query = NULL, *g_query_read;

    // BEGIN Query references
    struct key_value_refs refs[] = {
        { "offset", &(query->offset), key_string },
        { "q", &(query->query), key_string },
        { "code", &(query->code), key_string },
        { "type", &(query->type), key_int },
    };
    // END Query references
    
    if (query_string)
    {
        get_query = tb_malloc(strlen(query_string) + 1);
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
                {
                    refs[i].func(info.val, NULL, refs[i].val);
                    refs[i].val->is_set = 1;
                }
                else
                    refs[i].val->is_set = 0;
        }
        while (g_query_read);
    }

    return get_query;
}




char* read_post_data(REQUEST_T req, struct post_values* post)
{
    ptrdiff_t begin_curr_size;
    struct http_query_info query_info;
    struct http_form_info form_info;
    struct file_content form_props;
    char* request_method = GET_ENV("REQUEST_METHOD", req);
    char* content_length = GET_ENV("CONTENT_LENGTH", req);
    char* post_query = NULL, *p_query_read;

    // BEGIN Query references
    struct key_value_refs refs[] = {
        { "set", &(post->set), key_int },
        { "content", &(post->content), key_string },
        { "lang", &(post->lang), key_int },
        { "itype", &(post->itype), key_string },
        { "id", &(post->id), key_string },
        { "title", &(post->title), key_string },
        { "replies_policy", &(post->replies_policy), key_int },
        { "theme", &(post->theme), key_string },
        { "themeclr", &(post->themeclr), key_int },
        { "jsactions", &(post->jsactions), key_int },
        { "jsreply", &(post->jsreply), key_int },
        { "jslive", &(post->jslive), key_int },
        { "username", &(post->username), key_string },
        { "password", &(post->password), key_string },
        { "replyid", &(post->replyid), key_string },
        { "min_id", &(post->min_id), key_string },
        { "max_id", &(post->max_id), key_string },
        { "fileids", &(post->file_ids), key_string },
        { "start_id", &(post->start_id), key_string },
        { "instance", &(post->instance), key_string },
        { "visibility", &(post->visibility), key_string },
        { "emojoindex", &(post->emojoindex), key_int },
        { "only_media", &(post->only_media), key_int },
        { "replies_only", &(post->replies_only), key_int },
        { "interact_img", &(post->interact_img), key_int },
        { "js", &(post->js), key_int },
        { "statattachments", &(post->stat_attachments), key_int },
        { "statgreentexts", &(post->stat_greentexts), key_int },
        { "statdope", &(post->stat_dope), key_int },
        { "statoneclicksoftware", &(post->stat_oneclicksoftware), key_int },
        { "statemojolikes", &(post->stat_emojo_likes), key_int },
        { "stathidemuted", &(post->stat_hide_muted), key_int },
        { "instanceshowshoutbox", &(post->instance_show_shoutbox), key_int },
        { "sidebaropacity", &(post->sidebar_opacity), key_int },
        { "instancepanel", &(post->instance_panel), key_int },
        { "notifembed", &(post->notif_embed), key_int },
        { "file", &(post->files), key_files }
    };
    // END Query references

    if (request_method &&
        strcmp("POST", request_method) == 0 &&
        content_length)
    {
        char* mime_boundary;
        char* mime_mem = get_mime_boundary(GET_ENV("CONTENT_TYPE", req), &mime_boundary);
        int content_length = atoi(GET_ENV("CONTENT_LENGTH", req));
        post_query = tb_malloc(content_length + 1);
        if (!post_query)
        {
            perror("malloc");
            return NULL;
        }

        // fread should be a macro to FCGI_fread, which is set by FCGI_Accept in previous definitions
#ifndef CGI_MODE
        size_t len = FCGX_GetStr(post_query, content_length, req->in);
#else
        size_t len = fread(post_query, 1, content_length, stdin);
#endif
        post_query[content_length] = '\0';

        // For shifting through
        p_query_read = post_query;

        do
            if (mime_mem)
            {
                // Get size from here to the end
                begin_curr_size = p_query_read - post_query;
                // Mime value
                p_query_read = read_form_data(mime_boundary,
                                              p_query_read,
                                              &form_info,
                                              len - begin_curr_size);
                form_props.filename = form_info.filename;
                form_props.filetype = form_info.content_type;
                form_props.content_size = form_info.value_size;
                if (!p_query_read) break;
                for (size_t i = 0; i < (sizeof(refs)/sizeof(refs[0])); ++i)
                    if (strcmp(form_info.name, refs[i].key) == 0)
                    {
                        refs[i].func(form_info.value, &form_props, refs[i].val);
                        refs[i].val->is_set = 1;
                    }
            }
            else
            {
                // Mime value not set
                p_query_read = parse_query(p_query_read, &query_info);
                if (!(query_info.key && query_info.val)) break;
                for (size_t i = 0; i < (sizeof(refs)/sizeof(refs[0])); ++i)
                    if (strcmp(query_info.key, refs[i].key) == 0)
                    {
                        refs[i].func(query_info.val, NULL, refs[i].val);
                        refs[i].val->is_set = 1;
                    }
            }
        while (p_query_read);

        if (mime_mem) tb_free(mime_mem);
    }

    // Free me afterwards!
    return post_query;
}

char* parse_query(char* begin, struct http_query_info* info)
{
    int end = 0;
    char* val_begin = NULL;
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

char* try_handle_post(REQUEST_T req, void (*call)(struct http_query_info*, void*), void* arg)
{
    char* request_method = GET_ENV("REQUEST_ETHOD", req);
    char* post_query = NULL, * p_query_read;
    struct http_query_info info;
    
    // Handle POST
    if (request_method && (strcmp("POST", request_method) == 0))
    {
        int content_length = atoi(GET_ENV("CONTENT_LENGTH", req));
        post_query = tb_malloc(content_length + 1);
        if (!post_query)
        {
            puts("Malloc error!");
            return NULL;
        }
#ifdef CGI_MODE
        int size = read(STDIN_FILENO, post_query, content_length);
#else
        int size = FCGX_GetStr(post_query, content_length, req->in);
#endif
        if (size != content_length)
            return NULL;
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

void free_files(struct file_array* files)
{
    if (!files) return;
    struct file_content* content = files->content;
    for (size_t i = 0; i < files->array_size; ++i)
    {
        tb_free(content[i].content);
    }
    tb_free(content);
}

// TODO use hvstores_XXX macros
HV* perlify_post_values(struct post_values* post)
{
    HV* ssn_post_hv = newHV();

    // This ugly...
    hv_stores(ssn_post_hv, "theme", newSVpv(keystr(post->theme), 0));
    hv_stores(ssn_post_hv, "themeclr", newSViv(keyint(post->themeclr)));
    hv_stores(ssn_post_hv, "lang", newSViv(keyint(post->lang)));
    hv_stores(ssn_post_hv, "title", newSViv(keyint(post->title)));
    hv_stores(ssn_post_hv, "jsactions", newSViv(keyint(post->jsactions)));
    hv_stores(ssn_post_hv, "jsreply", newSViv(keyint(post->jsreply)));
    hv_stores(ssn_post_hv, "jslive", newSViv(keyint(post->jslive)));
    hv_stores(ssn_post_hv, "js", newSViv(keyint(post->js)));
    hv_stores(ssn_post_hv, "interact_img", newSViv(keyint(post->interact_img)));
    hv_stores(ssn_post_hv, "stat_attachments", newSViv(keyint(post->stat_attachments)));
    hv_stores(ssn_post_hv, "stat_greentexts", newSViv(keyint(post->stat_greentexts)));
    hv_stores(ssn_post_hv, "stat_dope", newSViv(keyint(post->stat_dope)));
    hv_stores(ssn_post_hv, "stat_oneclicksoftware", newSViv(keyint(post->stat_oneclicksoftware)));
    hv_stores(ssn_post_hv, "stat_emojo_likes", newSViv(keyint(post->stat_emojo_likes)));
    hv_stores(ssn_post_hv, "stat_hide_muted", newSViv(keyint(post->stat_hide_muted)));
    hv_stores(ssn_post_hv, "instance_show_shoutbox", newSViv(keyint(post->instance_show_shoutbox)));
    hv_stores(ssn_post_hv, "instance_panel", newSViv(keyint(post->instance_panel)));
    hv_stores(ssn_post_hv, "notif_embed", newSViv(keyint(post->notif_embed)));
    hv_stores(ssn_post_hv, "set", newSViv(keyint(post->set)));
    hv_stores(ssn_post_hv, "only_media", newSViv(keyint(post->only_media)));
    hv_stores(ssn_post_hv, "replies_only", newSViv(keyint(post->replies_only)));
    hv_stores(ssn_post_hv, "replies_policy", newSViv(keyint(post->replies_policy)));
    hv_stores(ssn_post_hv, "emojoindex", newSViv(keyint(post->emojoindex)));
    hv_stores(ssn_post_hv, "sidebar_opacity", newSViv(keyint(post->sidebar_opacity)));
    hv_stores(ssn_post_hv, "file_ids", newSVpv(keystr(post->file_ids), 0));
    hv_stores(ssn_post_hv, "content", newSVpv(keystr(post->content), 0));
    hv_stores(ssn_post_hv, "itype", newSVpv(keystr(post->itype), 0));
    hv_stores(ssn_post_hv, "id", newSVpv(keystr(post->id), 0));
    hv_stores(ssn_post_hv, "username", newSVpv(keystr(post->username), 0));
    hv_stores(ssn_post_hv, "password", newSVpv(keystr(post->password), 0));
    hv_stores(ssn_post_hv, "replyid", newSVpv(keystr(post->replyid), 0));
    hv_stores(ssn_post_hv, "visibility", newSVpv(keystr(post->visibility), 0));
    hv_stores(ssn_post_hv, "instance", newSVpv(keystr(post->instance), 0));
    hv_stores(ssn_post_hv, "min_id", newSVpv(keystr(post->min_id), 0));
    hv_stores(ssn_post_hv, "max_id", newSVpv(keystr(post->max_id), 0));
    hv_stores(ssn_post_hv, "start_id", newSVpv(keystr(post->start_id), 0));

    return ssn_post_hv;
}

HV* perlify_get_values(struct get_values* get)
{
    HV* ssn_query_hv = newHV();

    hv_stores(ssn_query_hv, "offset", newSVpv(keystr(get->offset), 0));
    hv_stores(ssn_query_hv, "query", newSVpv(keystr(get->query), 0));
    hv_stores(ssn_query_hv, "code", newSVpv(keystr(get->code), 0));
    hvstores_int(ssn_query_hv, "type", keyint(get->type));
    
    return ssn_query_hv;
}
