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

#include <stdlib.h>
#include <string.h>
#include <pcre.h>
#include "http.h"
#include "base_page.h"
#include "status.h"
#include "easprintf.h"
#include "query.h"
#include "cookie.h"
#include "string_helpers.h"
#include "error.h"
#include "reply.h"
#include "attachments.h"
#include "emoji_reaction.h"
#include "../config.h"
#include "type_string.h"
#include "string.h"
#include "emoji.h"

// Pages
#include "../static/status.chtml"
#include "../static/notification.chtml"
#include "../static/in_reply_to.chtml"

#define NUM_STR "%u"

struct status_args
{
    mastodont_t* api;
    struct mstdnt_status* status;
};

int try_post_status(struct session* ssn, mastodont_t* api)
{
    if (!(ssn->post.content)) return 1;

    struct mstdnt_storage storage, *att_storage = NULL;

    char** files;
    size_t files_len;
    struct mstdnt_attachment* attachments = NULL;
    char** media_ids = NULL;

    // Upload images
    try_upload_media(&att_storage, ssn, api, &attachments, &media_ids);

    // Cookie copy and read
    struct mstdnt_args args = {
        .content_type = "text/plain",
        .expires_in = 0,
        .in_reply_to_conversation_id = NULL,
        .in_reply_to_id = ssn->post.replyid,
        .language = NULL,
        .media_ids = media_ids,
        .media_ids_len = media_ids ? ssn->post.files.array_size : 0,
        .poll = NULL,
        .preview = 0,
        .scheduled_at = NULL,
        .sensitive = 0,
        .spoiler_text = NULL,
        .status = ssn->post.content,
        .visibility = ssn->post.visibility,
    };

    mastodont_create_status(api, &args, &storage);

    // TODO cleanup when errors are properly implemented
    mastodont_storage_cleanup(&storage);
    if (att_storage)
        cleanup_media_storages(ssn, att_storage);
    cleanup_media_ids(ssn, media_ids);
    if (attachments) free(attachments);
    
    return 0;
}

void content_status_create(struct session* ssn, mastodont_t* api, char** data)
{
    char* referer = getenv("HTTP_REFERER");

    try_post_status(ssn, api);

    redirect(REDIRECT_303, referer);
}

int try_interact_status(struct session* ssn, mastodont_t* api, char* id)
{
    struct mstdnt_storage storage = { 0 };
    if (!(ssn->post.itype && id)) return 1;

    // Pretty up the type
    if (strcmp(ssn->post.itype, "like") == 0 ||
        strcmp(ssn->post.itype, "likeboost") == 0)
        mastodont_favourite_status(api, id, &storage, NULL);
    // Not else if because possibly a like-boost
    if (strcmp(ssn->post.itype, "repeat") == 0 ||
        strcmp(ssn->post.itype, "likeboost") == 0)
        mastodont_reblog_status(api, id, &storage, NULL);
    else if (strcmp(ssn->post.itype, "bookmark") == 0)
        mastodont_bookmark_status(api, id, &storage, NULL);
    else if (strcmp(ssn->post.itype, "pin") == 0)
        mastodont_pin_status(api, id, &storage, NULL);
    else if (strcmp(ssn->post.itype, "mute") == 0)
        mastodont_mute_conversation(api, id, &storage, NULL);
    else if (strcmp(ssn->post.itype, "unlike") == 0)
        mastodont_unfavourite_status(api, id, &storage, NULL);
    else if (strcmp(ssn->post.itype, "unrepeat") == 0)
        mastodont_unreblog_status(api, id, &storage, NULL);
    else if (strcmp(ssn->post.itype, "unbookmark") == 0)
        mastodont_unbookmark_status(api, id, &storage, NULL);
    else if (strcmp(ssn->post.itype, "unpin") == 0)
        mastodont_unpin_status(api, id, &storage, NULL);
    else if (strcmp(ssn->post.itype, "unmute") == 0)
        mastodont_unmute_conversation(api, id, &storage, NULL);

    mastodont_storage_cleanup(&storage);

    return 0;
}

char* construct_in_reply_to(mastodont_t* api, struct mstdnt_status* status, size_t* size)
{
    char* irt_html;
    size_t s;
    struct mstdnt_storage storage;
    struct mstdnt_account acct;

    mastodont_get_account(api,
                          config_experimental_lookup,
                          status->in_reply_to_account_id,
                          &acct,
                          &storage,
                          NULL);

    s = easprintf(&irt_html, data_in_reply_to_html,
                  config_url_prefix,
                  status->in_reply_to_id,
                  L10N[L10N_EN_US][L10N_IN_REPLY_TO],
                  acct.acct);
    
    if (size) *size = s;

    mastodont_storage_cleanup(&storage);
    return irt_html;
}

#define REGEX_GREENTEXT "((?:^|<br/?>|\\s)&gt;.*?)(?:<br/?>|$)"
#define REGEX_GREENTEXT_LEN 6

char* reformat_status(char* content, struct mstdnt_emoji* emos, size_t emos_len)
{
    char* gt_res;
    char* emo_res;
    gt_res = greentextify(content);
    if (emos)
    {
        emo_res = emojify(gt_res, emos, emos_len);
        free(gt_res);
        return emo_res;
    }
    return gt_res;
}

char* greentextify(char* content)
{
    const char* error;
    int erroffset;
    int rc;
    int gt_off;
    int gt_len;
    char* res = content;

    // Malloc'd strings
    char* reg_string;
    char* gt_string;
    
    char* oldres = NULL;
    int re_results[REGEX_GREENTEXT_LEN];
    pcre* re = pcre_compile(REGEX_GREENTEXT, 0, &error, &erroffset, NULL);
    if (re == NULL)
    {
        fprintf(stderr, "Couldn't parse regex at offset %d: %s\n", erroffset, error);
        pcre_free(re);
        return res;
    }

    for (int ind = 0;;)
    {
        rc = pcre_exec(re, NULL, res, strlen(res), ind, 0, re_results, REGEX_GREENTEXT_LEN);
        if (rc < 0)
            break;

        // Store to last result
        gt_off = re_results[2];
        gt_len = re_results[3] - gt_off;

        oldres = res;

        // Malloc find/repl strings
        reg_string = malloc(gt_len + 1);
        strncpy(reg_string, res + gt_off, gt_len);
        reg_string[gt_len] = '\0';
        easprintf(&gt_string, "<span class=\"greentext\">%s</span>", reg_string);
        
        res = strrepl(res, reg_string, gt_string, 0);
        if (oldres != content) free(oldres);
        ind = re_results[2] + strlen(gt_string);
        free(reg_string);
        free(gt_string);
    }

    pcre_free(re);
    return res;
}

char* construct_status(mastodont_t* api,
                       struct mstdnt_status* local_status,
                       int* size,
                       struct mstdnt_notification* local_notif,
                       uint8_t flags)
{
    char* stat_html;

    // Counts
    char* reply_count = NULL;
    char* repeat_count = NULL;
    char* favourites_count = NULL;
    char* attachments = NULL;
    char* emoji_reactions = NULL;
    char* notif_info = NULL;
    char* in_reply_to_str = NULL;
    struct mstdnt_status* status = local_status;
    // Create a "fake" notification header which contains information for
    // the reblogged status
    struct mstdnt_notification notif_reblog;
    struct mstdnt_notification* notif = local_notif;

    // Repoint value if it's a reblog
    if (status->reblog)
    {
        status = status->reblog;
        // Point to our account
        notif_reblog.account = &(local_status->account);
        notif_reblog.type = MSTDNT_NOTIFICATION_REBLOG;
        notif = &notif_reblog;
    }
    char* parse_content = reformat_status(status->content, status->emojis, status->emojis_len);
    
    if (status->replies_count)
        easprintf(&reply_count, NUM_STR, status->replies_count);
    if (status->reblogs_count)
        easprintf(&repeat_count, NUM_STR, status->reblogs_count);
    if (status->favourites_count)
        easprintf(&favourites_count, NUM_STR, status->favourites_count);
    if (status->media_attachments_len)
        attachments = construct_attachments(status->sensitive, status->media_attachments, status->media_attachments_len, NULL);
    if (status->pleroma.emoji_reactions_len)
        emoji_reactions = construct_emoji_reactions(status->pleroma.emoji_reactions, status->pleroma.emoji_reactions_len, NULL);
    if (notif && notif->type != MSTDNT_NOTIFICATION_MENTION)
        easprintf(&notif_info, data_notification_html,
                  notif->account->avatar,
                  notif->account->display_name,
                  // Use compact type string, which looks better
                  // for self-boosts
                  local_status->reblog ? notification_type_compact_str(notif->type) : notification_type_str(notif->type),
                  notification_type_svg(notif->type));

    if (status->in_reply_to_id && status->in_reply_to_account_id)
        in_reply_to_str = construct_in_reply_to(api, status, NULL);

    size_t s = easprintf(&stat_html, data_status_html,
                         status->id,
                         (flags & STATUS_FOCUSED) == STATUS_FOCUSED ?
                         "focused" : "",
                         notif_info ? notif_info : "",
                         status->account.avatar,
                         status->account.display_name, /* Username */
                         config_url_prefix,
                         status->account.acct,
                         status->account.acct, /* Account */
                         status->visibility, /* visibility */
                         config_url_prefix,
                         status->id,
                         status->muted ? "un" : "",
                         status->muted ? "Unmute thread" : "Mute thread",
                         config_url_prefix,
                         status->id,
                         status->pinned ? "un" : "",
                         status->pinned ? "Unpin" : "Pin",
                         config_url_prefix,
                         status->id,
                         status->bookmarked ? "un" : "",
                         status->bookmarked ? "Remove Bookmark" : "Bookmark",
                         in_reply_to_str ? in_reply_to_str : "",
                         parse_content,
                         attachments ? attachments : "",
                         emoji_reactions ? emoji_reactions : "",
                         config_url_prefix,
                         status->id,
                         status->id,
                         reply_count ? reply_count : "",
                         config_url_prefix,
                         status->id,
                         status->reblogged ? "un" : "",
                         status->reblogged ? "active" : "",
                         repeat_count ? repeat_count : "",
                         config_url_prefix,
                         status->id,
                         status->favourited ? "un" : "",
                         status->favourited ? "active" : "",
                         favourites_count ? favourites_count : "",
                         config_url_prefix,
                         status->id,
                         config_url_prefix,
                         status->id,
                         status->id);
    
    if (size) *size = s;
    // Cleanup
    if (reply_count) free(reply_count);
    if (repeat_count) free(repeat_count);
    if (favourites_count) free(favourites_count);
    if (in_reply_to_str) free(in_reply_to_str);
    if (attachments) free(attachments);
    if (emoji_reactions) free(emoji_reactions);
    if (notif) free(notif_info);
    if (parse_content != status->content) free(parse_content);
    return stat_html;
}

static char* construct_status_voidwrap(void* passed, size_t index, int* res)
{
    struct status_args* args = passed;
    return construct_status(args->api, args->status + index, res, NULL, 0);
}

char* construct_statuses(mastodont_t* api, struct mstdnt_status* statuses, size_t size, size_t* ret_size)
{
    struct status_args args = {
        .api = api,
        .status = statuses,
    };
    return construct_func_strings(construct_status_voidwrap, &args, size, ret_size);
}

void status_interact(struct session* ssn, mastodont_t* api, char** data)
{
    char* referer = getenv("HTTP_REFERER");
    
    try_interact_status(ssn, api, data[0]);
    
    printf("Status: 303 See Other\r\n"
           "Location: %s#id-%s\r\n"
           "Content-Length: 14\r\n\r\n"
           "Redirecting...",
           referer ? referer : "/",
           data[0]);
}

void status_view(struct session* ssn, mastodont_t* api, char** data)
{
    content_status(ssn, api, data, 0);
}

void status_reply(struct session* ssn, mastodont_t* api, char** data)
{
    content_status(ssn, api, data, 1);
}

void content_status(struct session* ssn, mastodont_t* api, char** data, int is_reply)
{
    char* output;
    // Status context
    struct mstdnt_storage storage, status_storage;
    struct mstdnt_status* statuses_before, *statuses_after, status = { 0 };
    size_t stat_before_len, stat_after_len;
    char* before_html = NULL, *stat_html = NULL, *after_html = NULL, *stat_reply = NULL;

    try_post_status(ssn, api);
    mastodont_get_status_context(api, data[0], &storage, &statuses_before, &statuses_after,
                                 &stat_before_len, &stat_after_len);
    
    // Get information
    if (mastodont_get_status(api, data[0], &status_storage, &status))
    {
        stat_html = construct_error("Status not found", E_ERROR, 1, NULL);
    }
    else {
        before_html = construct_statuses(api, statuses_before, stat_before_len, NULL);

        // Current status
        stat_html = construct_status(api, &status, NULL, NULL, STATUS_FOCUSED);
        if (is_reply)
        {
            stat_reply = reply_status(data[0],
                                      &status);
        }
    }

    // After...
    after_html = construct_statuses(api, statuses_after, stat_after_len, NULL);

    easprintf(&output, "%s%s%s%s",
              before_html ? before_html : "",
              stat_html ? stat_html : "",
              stat_reply ? stat_reply : "",
              after_html ? after_html : "");
    
    struct base_page b = {
        .category = BASE_CAT_NONE,
        .locale = L10N_EN_US,
        .content = output,
        .sidebar_left = NULL
    };

    // Output
    render_base_page(&b, ssn, api);

    // Cleanup
    if (before_html) free(before_html);
    if (stat_html) free(stat_html);
    if (after_html) free(after_html);
    if (output) free(output);
    if (is_reply) free(stat_reply);
    mstdnt_cleanup_statuses(statuses_before, stat_before_len);
    mstdnt_cleanup_statuses(statuses_after, stat_after_len);
    mstdnt_cleanup_status(&status);
    mastodont_storage_cleanup(&storage);
    mastodont_storage_cleanup(&status_storage);
}

void notice_redirect(struct session* ssn, mastodont_t* api, char** data)
{
    char* url;
    easprintf(&url, "%s/status/%s", config_url_prefix, data[0]);
    redirect(REDIRECT_303, url);
    free(url);
}
