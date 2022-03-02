/*
 * RatFE - Lightweight frontend for Pleroma
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
#include "base_page.h"
#include "status.h"
#include "easprintf.h"
#include "query.h"
#include "cookie.h"
#include "string_helpers.h"
#include "reply.h"
#include "attachments.h"
#include "emoji_reaction.h"
#include "../config.h"

// Pages
#include "../static/status.chtml"

#define NUM_STR " (%u)"

int try_post_status(mastodont_t* api)
{
    if (!post.content) return 1;

    struct mstdnt_storage storage;

    // Cookie copy and read
    struct mstdnt_create_status_args args = {
        .content_type = "text/plain",
        .expires_in = 0,
        .in_reply_to_conversation_id = NULL,
        .in_reply_to_id = post.replyid,
        .language = NULL,
        .media_ids = NULL,
        .poll = NULL,
        .preview = 0,
        .scheduled_at = NULL,
        .sensitive = 0,
        .spoiler_text = NULL,
        .status = post.content,
        .visibility = "public",
    };

    mastodont_create_status(api, &args, &storage);

    // TODO cleanup when errors are properly implemented
    mastodont_storage_cleanup(&storage);
    
    return 0;
}

int try_interact_status(mastodont_t* api, char* id)
{
    struct mstdnt_storage storage;
    if (!(post.itype && id)) return 1;

    // Pretty up the type
    if (strcmp(post.itype, "like") == 0)
    {
        mastodont_favourite_status(api,
                                   id,
                                   &storage);
        // TODO Cleanup when errors handled
        // mastodont_storage_cleanup(&storage);
    }

    return 0;
}

char* construct_status(struct mstdnt_status* status, int* size)
{
    char* stat_html;

    // Counts
    char* reply_count = NULL;
    char* repeat_count = NULL;
    char* favourites_count = NULL;
    char* attachments = NULL;
    char* emoji_reactions = NULL;
    if (status->replies_count)
        easprintf(&reply_count, NUM_STR, status->replies_count);
    if (status->reblogs_count)
        easprintf(&repeat_count, NUM_STR, status->reblogs_count);
    if (status->favourites_count)
        easprintf(&favourites_count, NUM_STR, status->favourites_count);
    if (status->media_attachments_len)
        attachments = construct_attachments(status->media_attachments, status->media_attachments_len, NULL);
    if (status->pleroma.emoji_reactions_len)
        emoji_reactions = construct_emoji_reactions(status->pleroma.emoji_reactions, status->pleroma.emoji_reactions_len, NULL);
        

    size_t s = easprintf(&stat_html, data_status_html,
                         status->account.avatar,
                         status->account.display_name, /* Username */
                         config_url_prefix,
                         status->account.acct,
                         status->account.acct, /* Account */
                         "Public", /* visibility */
                         status->content,
                         attachments ? attachments : "",
                         emoji_reactions ? emoji_reactions : "",
                         config_url_prefix,
                         status->id,
                         reply_count ? reply_count : "",
                         config_url_prefix,
                         status->id,
                         status->reblogged ? "nobutton-active" : "",
                         repeat_count ? repeat_count : "",
                         config_url_prefix,
                         status->id,
                         status->favourited ? "nobutton-active" : "",
                         favourites_count ? favourites_count : "",
                         config_url_prefix,
                         status->id,
                         config_url_prefix,
                         status->id);
    if (size) *size = s;
    // Cleanup
    if (reply_count) free(reply_count);
    if (repeat_count) free(repeat_count);
    if (favourites_count) free(favourites_count);
    if (attachments) free(attachments);
    if (emoji_reactions) free(emoji_reactions);
    return stat_html;
}

static char* construct_status_voidwrap(void* passed, size_t index, int* res)
{
    return construct_status((struct mstdnt_status*)passed + index, res);
}

char* construct_statuses(struct mstdnt_status* statuses, size_t size, size_t* ret_size)
{
    return construct_func_strings(construct_status_voidwrap, statuses, size, ret_size);
}

void status_interact(mastodont_t* api, char** data, size_t data_size)
{
    char* referer = getenv("HTTP_REFERER");
    
    try_interact_status(api, data[0]);
    
    fputs("Status: 302 Found\r\n", stdout);
    printf("Location: %s\r\n\r\nRedirecting...",
           referer ? referer : "/");
}

void status_view(mastodont_t* api, char** data, size_t data_size)
{
    content_status(api, data, data_size, 0);
}

void status_reply(mastodont_t* api, char** data, size_t data_size)
{
    content_status(api, data, data_size, 1);
}

void content_status(mastodont_t* api, char** data, size_t data_size, int is_reply)
{
    char* output;
    // Status context
    struct mstdnt_storage storage, status_storage;
    struct mstdnt_status* statuses_before, *statuses_after, status;
    size_t stat_before_len, stat_after_len;
    char* before_html = NULL, *stat_html = NULL, *after_html = NULL, *stat_reply = NULL;

    try_post_status(api);
    
#ifdef _TEST_
#include "test/status_test.h"
#else
    // Get information
    mastodont_status_context(api, data[0], &storage, &statuses_before, &statuses_after,
                             &stat_before_len, &stat_after_len);
    mastodont_view_status(api, data[0], &status_storage, &status);

    // Before...
    before_html = construct_statuses(statuses_before, stat_before_len, NULL);

    // Current status
    stat_html = construct_status(&status, NULL);
    if (is_reply)
    {
        stat_reply = reply_status(data[0],
                                  &status);
    }

    // After...
    after_html = construct_statuses(statuses_after, stat_after_len, NULL);
#endif

    easprintf(&output, "%s%s%s%s",
              before_html ? before_html : "",
              stat_html ? stat_html : "",
              stat_reply ? stat_reply : "",
              after_html ? after_html : "");
    
    struct base_page b = {
        .locale = L10N_EN_US,
        .content = output,
        .sidebar_right = NULL
    };

    // Output
    render_base_page(&b);

    // Cleanup
    if (before_html) free(before_html);
    if (stat_html) free(stat_html);
    if (after_html) free(after_html);
    if (output) free(output);
    if (is_reply) free(stat_reply);
    cleanup_statuses(statuses_before, stat_before_len);
    cleanup_statuses(statuses_after, stat_after_len);
    cleanup_status(&status);
    mastodont_storage_cleanup(&storage);
}
