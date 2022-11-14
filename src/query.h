/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef QUERY_H
#define QUERY_H
#include "global_perl.h"
#include <fcgi_stdio.h>
#include <stddef.h>
#include "key.h"
#include "request.h"

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
    struct key interact_img; // Int
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
    struct key replies_policy; // Int
    
    struct key file_ids; // String
    struct key content; // String
    struct key itype; // String
    struct key id; // String
    struct key username; // String
    struct key password; // String
    struct key replyid; // String
    struct key visibility; // String
    struct key instance; // String
    struct key emojoindex; // Int
    struct key sidebar_opacity; // Int

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
    struct key type; // Int
};

char* read_get_data(REQUEST_T req, struct get_values* query);
char* read_post_data(REQUEST_T req, struct post_values* post);
/* A stupidly quick query parser */
char* parse_query(char* begin, struct http_query_info* info);
char* try_handle_post(REQUEST_T req, void (*call)(struct http_query_info*, void*), void* arg);

void free_files(struct file_array* files);

// Perl stuff
HV* perlify_post_values(struct post_values* post);
HV* perlify_get_values(struct get_values* get);

#endif // QUERY_H
